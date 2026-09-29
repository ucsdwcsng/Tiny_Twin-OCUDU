/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include "oai_cuda.h"


__global__ void multipath_channel_kernel_batched(const float2 *__restrict__ d_channel_coeffs,
                                                 const float2 *__restrict__ tx_sig,
                                                 float2 *__restrict__ rx_sig,
                                                 int num_samples,
                                                 int channel_length,
                                                 int nb_tx,
                                                 int nb_rx);

__global__ void add_noise_and_phase_noise_kernel_batched(const float2 *__restrict__ r_sig,
                                                         short2 *__restrict__ output_sig,
                                                         curandState_t *states,
                                                         int num_samples,
                                                         int nb_rx,
                                                         float sigma,
                                                         float pn_std_dev,
                                                         bool apply_phase_noise);

#define CHECK_CUDA(val) checkCuda((val), #val, __FILE__, __LINE__)
static void checkCuda(cudaError_t result, const char *const func, const char *const file, const int line)
{
  if (result != cudaSuccess) {
    fprintf(stderr,
            "CUDA Error at %s:%d code=%d(%s) \"%s\" \n",
            file,
            line,
            static_cast<unsigned int>(result),
            cudaGetErrorString(result),
            func);
    cudaDeviceReset();
    exit(EXIT_FAILURE);
  }
}

__global__ void sum_outputs_kernel(const short2 *__restrict__ *__restrict__ individual_outputs,
                                   short2 *__restrict__ final_summed_output,
                                   int num_channels,
                                   int num_samples_per_antenna)
{
  const int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i >= num_samples_per_antenna)
    return;

  float2 sum = make_float2(0.0f, 0.0f);

  for (int c = 0; c < num_channels; c++) {
    sum.x += individual_outputs[c][i].x;
    sum.y += individual_outputs[c][i].y;
  }

  final_summed_output[i].x = (short)fmaxf(-32768.0f, fminf(32767.0f, sum.x));
  final_summed_output[i].y = (short)fmaxf(-32768.0f, fminf(32767.0f, sum.y));
}

extern "C" {

void init_cuda_chsim_buffers(int use_cuda,
                             int n_tx,
                             int n_rx,
                             void **d_tx_sig,
                             void **d_intermediate_sig,
                             void **d_final_output,
                             void **d_curand_states,
                             void **h_tx_sig_pinned,
                             void **h_final_output_pinned,
                             void **d_channel_coeffs_gpu)
{
  if (use_cuda) {
    int num_samples_alloc = 153600;

#if defined(USE_UNIFIED_MEMORY)
    printf("Allocating CUDA Unified Memory...\n");
    const int max_padding_alloc = 256 - 1;
    size_t padded_tx_alloc_bytes = n_tx * (num_samples_alloc + max_padding_alloc) * 2 * sizeof(float);
    CHECK_CUDA(cudaMallocManaged(d_tx_sig, padded_tx_alloc_bytes, cudaMemAttachGlobal));
    CHECK_CUDA(cudaMallocManaged(d_intermediate_sig, n_rx * num_samples_alloc * sizeof(float) * 2, cudaMemAttachGlobal));
    CHECK_CUDA(cudaMallocManaged(d_final_output, n_rx * num_samples_alloc * sizeof(short) * 2, cudaMemAttachGlobal));
    *h_tx_sig_pinned = *d_tx_sig;
    *h_final_output_pinned = *d_final_output;
#if defined(CUDA_VERSION) && CUDA_VERSION >= 13000
    cudaMemLocation deviceId;
    deviceId.type = cudaMemLocationTypeDevice;
    CHECK_CUDA(cudaGetDevice(&deviceId.id));
    cudaMemLocation cpuDeviceId;
    cpuDeviceId.id = cudaCpuDeviceId;
    cpuDeviceId.type = cudaMemLocationTypeHost;
#else
    int deviceId;
    CHECK_CUDA(cudaGetDevice(&deviceId));
    int cpuDeviceId = cudaCpuDeviceId;
#endif
    CHECK_CUDA(cudaMemAdvise(*d_tx_sig, padded_tx_alloc_bytes, cudaMemAdviseSetReadMostly, deviceId));
    CHECK_CUDA(cudaMemAdvise(*d_intermediate_sig,
                             n_rx * num_samples_alloc * sizeof(float) * 2,
                             cudaMemAdviseSetPreferredLocation,
                             deviceId));
    CHECK_CUDA(
        cudaMemAdvise(*d_final_output, n_rx * num_samples_alloc * sizeof(short) * 2, cudaMemAdviseSetPreferredLocation, deviceId));
    CHECK_CUDA(
        cudaMemAdvise(*d_final_output, n_rx * num_samples_alloc * sizeof(short) * 2, cudaMemAdviseSetAccessedBy, cpuDeviceId));

#elif defined(USE_ATS_MEMORY)
    printf("Allocating memory for ATS Hybrid path...\n");
    const int max_padding_alloc = 256 - 1;
    size_t padded_tx_alloc_bytes = n_tx * (num_samples_alloc + max_padding_alloc) * 2 * sizeof(float);
    *h_tx_sig_pinned = malloc(padded_tx_alloc_bytes);
    CHECK_CUDA(cudaMalloc(d_intermediate_sig, n_rx * num_samples_alloc * sizeof(float) * 2));
    CHECK_CUDA(cudaMalloc(d_final_output, n_rx * num_samples_alloc * sizeof(short) * 2));
    *h_final_output_pinned = malloc(n_rx * num_samples_alloc * sizeof(short) * 2);
    *d_tx_sig = NULL;
#else // Explicit copy method
    printf("Pre-allocating GPU & Pinned memory for the channel pipeline...\n");
    const int max_padding_alloc = 256 - 1;
    size_t padded_tx_alloc_bytes = n_tx * (num_samples_alloc + max_padding_alloc) * 2 * sizeof(float);
    CHECK_CUDA(cudaMalloc(d_tx_sig, padded_tx_alloc_bytes));
    CHECK_CUDA(cudaMalloc(d_intermediate_sig, n_rx * num_samples_alloc * sizeof(float) * 2));
    CHECK_CUDA(cudaMalloc(d_final_output, n_rx * num_samples_alloc * sizeof(short) * 2));
    CHECK_CUDA(cudaMallocHost(h_tx_sig_pinned, padded_tx_alloc_bytes));
    CHECK_CUDA(cudaMallocHost(h_final_output_pinned, n_rx * num_samples_alloc * sizeof(short) * 2));
#endif
    const int max_taps_alloc = 256; // Upper bound for channel length
    size_t channel_buffer_size = n_tx * n_rx * max_taps_alloc * sizeof(float) * 2;
    CHECK_CUDA(cudaMalloc(d_channel_coeffs_gpu, channel_buffer_size));
    int num_rand_elements = n_rx * num_samples_alloc;
    *d_curand_states = create_and_init_curand_states_cuda(num_rand_elements, time(NULL));
  }
}

void free_cuda_chsim_buffers(int use_cuda,
                             void **d_tx_sig_p,
                             void **d_intermediate_sig_p,
                             void **d_final_output_p,
                             void **d_curand_states_p,
                             void **h_tx_sig_pinned_p,
                             void **h_final_output_pinned_p,
                             float **h_channel_coeffs_p,
                             void **d_channel_coeffs_gpu_p)
{
  if (use_cuda) {
    printf("Freeing GPU and Pinned memory...\n");
#if defined(USE_UNIFIED_MEMORY)
    if (*d_tx_sig_p) {
      cudaFree(*d_tx_sig_p);
      *d_tx_sig_p = NULL;
      *h_tx_sig_pinned_p = NULL;
    }
    if (*d_intermediate_sig_p) {
      cudaFree(*d_intermediate_sig_p);
      *d_intermediate_sig_p = NULL;
    }
    if (*d_final_output_p) {
      cudaFree(*d_final_output_p);
      *d_final_output_p = NULL;
      *h_final_output_pinned_p = NULL;
    }
#elif defined(USE_ATS_MEMORY)
    if (*h_tx_sig_pinned_p) {
      free(*h_tx_sig_pinned_p);
      *h_tx_sig_pinned_p = NULL;
    }
    if (*d_intermediate_sig_p) {
      cudaFree(*d_intermediate_sig_p);
      *d_intermediate_sig_p = NULL;
    }
    if (*d_final_output_p) {
      cudaFree(*d_final_output_p);
      *d_final_output_p = NULL;
    }
    if (*h_final_output_pinned_p) {
      free(*h_final_output_pinned_p);
      *h_final_output_pinned_p = NULL;
    }
#else
    if (*d_tx_sig_p) {
      cudaFree(*d_tx_sig_p);
      *d_tx_sig_p = NULL;
    }
    if (*d_intermediate_sig_p) {
      cudaFree(*d_intermediate_sig_p);
      *d_intermediate_sig_p = NULL;
    }
    if (*d_final_output_p) {
      cudaFree(*d_final_output_p);
      *d_final_output_p = NULL;
    }
    if (*h_tx_sig_pinned_p) {
      cudaFreeHost(*h_tx_sig_pinned_p);
      *h_tx_sig_pinned_p = NULL;
    }
    if (*h_final_output_pinned_p) {
      cudaFreeHost(*h_final_output_pinned_p);
      *h_final_output_pinned_p = NULL;
    }
#endif
    if (*d_channel_coeffs_gpu_p) {
      cudaFree(*d_channel_coeffs_gpu_p);
      *d_channel_coeffs_gpu_p = NULL;
    }
    destroy_curand_states_cuda(*d_curand_states_p);
    *d_curand_states_p = NULL;
    if (*h_channel_coeffs_p) {
      free(*h_channel_coeffs_p);
      *h_channel_coeffs_p = NULL;
    }
  } else {
    if (*h_tx_sig_pinned_p) {
      free(*h_tx_sig_pinned_p);
      *h_tx_sig_pinned_p = NULL;
    }
  }
}

void run_channel_pipeline_cuda(c16_t **output_signal,
                               int nb_tx,
                               int nb_rx,
                               int channel_length,
                               uint32_t num_samples,
                               float *h_channel_coeffs,
                               float sigma2,
                               double ts,
                               uint16_t pdu_bit_map,
                               uint16_t ptrs_bit_map,
                               int slot_offset,
                               int delay,
                               void *d_tx_sig_void,
                               void *d_intermediate_sig_void,
                               void *d_final_output_void,
                               void *d_curand_states_void,
                               void *h_tx_sig_pinned_void,
                               void *h_final_output_pinned_void,
                               void *d_channel_coeffs_void)
{
  // --- Cast void pointers ---
  float2 *d_intermediate_sig = (float2 *)d_intermediate_sig_void;
  short2 *d_final_output = (short2 *)d_final_output_void;
  curandState_t *d_curand_states = (curandState_t *)d_curand_states_void;
  float2 *d_channel_coeffs = (float2 *)d_channel_coeffs_void;

  const int padding_len = channel_length - 1;
  const size_t padded_stride_bytes = (num_samples + padding_len) * 2 * sizeof(float);
  const size_t total_padded_tx_bytes = nb_tx * padded_stride_bytes;

  float *kernel_input_ptr;
#if defined(USE_UNIFIED_MEMORY) || defined(USE_ATS_MEMORY)
  kernel_input_ptr = (float *)h_tx_sig_pinned_void;
#else
  float *d_tx_sig = (float *)d_tx_sig_void;
  float *h_tx_sig_pinned = (float *)h_tx_sig_pinned_void;
  CHECK_CUDA(cudaMemcpy(d_tx_sig, h_tx_sig_pinned, total_padded_tx_bytes, cudaMemcpyHostToDevice));

  kernel_input_ptr = d_tx_sig;
#endif

  size_t channel_size_bytes = nb_tx * nb_rx * channel_length * sizeof(float2);
  CHECK_CUDA(cudaMemcpy(d_channel_coeffs, h_channel_coeffs, channel_size_bytes, cudaMemcpyHostToDevice));

  dim3 threads_multipath(512, 1);
  dim3 blocks_multipath((num_samples + threads_multipath.x - 1) / threads_multipath.x, nb_rx);
  size_t sharedMemSize = (threads_multipath.x + channel_length - 1) * sizeof(float2);
  multipath_channel_kernel<<<blocks_multipath, threads_multipath, sharedMemSize>>>(d_channel_coeffs,
                                                                                   kernel_input_ptr,
                                                                                   d_intermediate_sig,
                                                                                   num_samples,
                                                                                   channel_length,
                                                                                   nb_tx,
                                                                                   nb_rx);

  dim3 threads_noise(256, 1);
  dim3 blocks_noise((num_samples + threads_noise.x - 1) / threads_noise.x, nb_rx);
  float pn_variance = 1e-5f * 2.0f * 3.1415926535f * 300.0f * (float)ts;
  bool apply_phase_noise = (pdu_bit_map & ptrs_bit_map);
  add_noise_and_phase_noise_kernel<<<blocks_noise, threads_noise>>>(d_intermediate_sig,
                                                                    d_final_output,
                                                                    d_curand_states,
                                                                    num_samples,
                                                                    sqrtf(sigma2 / 2.0f),
                                                                    sqrtf(pn_variance),
                                                                    apply_phase_noise);

  cudaDeviceSynchronize();

  // If output_signal is NULL, the caller intends to keep the data on the GPU
  // for further processing (e.g., summing outputs). Otherwise, copy back to host.
  if (output_signal != NULL) {
#if defined(USE_UNIFIED_MEMORY)
    short2 *h_final_output_pinned = (short2 *)h_final_output_pinned_void;
    for (int ii = 0; ii < nb_rx; ii++) {
      memcpy(output_signal[ii] + slot_offset + delay, h_final_output_pinned + ii * num_samples, num_samples * sizeof(short2));
    }
#else
    short2 *h_final_output_pinned = (short2 *)h_final_output_pinned_void;
    CHECK_CUDA(cudaMemcpy(h_final_output_pinned, d_final_output, nb_rx * num_samples * sizeof(short2), cudaMemcpyDeviceToHost));
    for (int ii = 0; ii < nb_rx; ii++) {
      memcpy(output_signal[ii] + slot_offset + delay, h_final_output_pinned + ii * num_samples, num_samples * sizeof(short2));
    }
#endif
  }
}

void sum_channel_outputs_cuda(void **d_individual_outputs, void *d_final_output, int num_channels, int nb_rx, int num_samples)
{
  void **d_ptr_array;
  size_t ptr_array_size = num_channels * sizeof(void *);
  CHECK_CUDA(cudaMalloc(&d_ptr_array, ptr_array_size));

  CHECK_CUDA(cudaMemcpy(d_ptr_array, d_individual_outputs, ptr_array_size, cudaMemcpyHostToDevice));

  int num_total_samples = nb_rx * num_samples;
  dim3 threads(256, 1);
  dim3 blocks((num_total_samples + threads.x - 1) / threads.x, 1);

  sum_outputs_kernel<<<blocks, threads>>>((const short2 **)d_ptr_array, (short2 *)d_final_output, num_channels, num_total_samples);

  CHECK_CUDA(cudaFree(d_ptr_array));
}

void run_channel_pipeline_cuda_streamed(int nb_tx,
                                        int nb_rx,
                                        int channel_length,
                                        uint32_t num_samples,
                                        float *h_channel_coeffs,
                                        float sigma2,
                                        double ts,
                                        uint16_t pdu_bit_map,
                                        uint16_t ptrs_bit_map,
                                        void *d_tx_sig_void,
                                        void *d_intermediate_sig_void,
                                        void *d_final_output_void,
                                        void *d_curand_states_void,
                                        void *h_tx_sig_pinned_void,
                                        void *d_channel_coeffs_void,
                                        void *stream_void)
{
  cudaStream_t stream = (cudaStream_t)stream_void;
  float2 *d_intermediate_sig = (float2 *)d_intermediate_sig_void;
  short2 *d_final_output = (short2 *)d_final_output_void;
  curandState_t *d_curand_states = (curandState_t *)d_curand_states_void;
  float2 *d_channel_coeffs = (float2 *)d_channel_coeffs_void;
  float *kernel_input_ptr;

#if defined(USE_UNIFIED_MEMORY) || defined(USE_ATS_MEMORY)
  kernel_input_ptr = (float *)h_tx_sig_pinned_void;
#else
  float *d_tx_sig = (float *)d_tx_sig_void;
  float *h_tx_sig_pinned = (float *)h_tx_sig_pinned_void;

  const int padding_len = channel_length - 1;
  const size_t total_padded_tx_bytes = nb_tx * (num_samples + padding_len) * 2 * sizeof(float);

  CHECK_CUDA(cudaMemcpyAsync(d_tx_sig, h_tx_sig_pinned, total_padded_tx_bytes, cudaMemcpyHostToDevice, stream));
  kernel_input_ptr = d_tx_sig;
#endif

  size_t channel_size_bytes = nb_tx * nb_rx * channel_length * sizeof(float2);
  CHECK_CUDA(cudaMemcpyAsync(d_channel_coeffs, h_channel_coeffs, channel_size_bytes, cudaMemcpyHostToDevice, stream));

  dim3 threads_multipath(512, 1);
  dim3 blocks_multipath((num_samples + threads_multipath.x - 1) / threads_multipath.x, nb_rx);
  size_t sharedMemSize = (threads_multipath.x + channel_length - 1) * sizeof(float2);
  multipath_channel_kernel<<<blocks_multipath, threads_multipath, sharedMemSize, stream>>>(d_channel_coeffs,
                                                                                           kernel_input_ptr,
                                                                                           d_intermediate_sig,
                                                                                           num_samples,
                                                                                           channel_length,
                                                                                           nb_tx,
                                                                                           nb_rx);

  dim3 threads_noise(256, 1);
  dim3 blocks_noise((num_samples + threads_noise.x - 1) / threads_noise.x, nb_rx);
  float pn_variance = 1e-5f * 2.0f * 3.1415926535f * 300.0f * (float)ts;
  bool apply_phase_noise = (pdu_bit_map & ptrs_bit_map);
  add_noise_and_phase_noise_kernel<<<blocks_noise, threads_noise, 0, stream>>>(d_intermediate_sig,
                                                                               d_final_output,
                                                                               d_curand_states,
                                                                               num_samples,
                                                                               sqrtf(sigma2 / 2.0f),
                                                                               sqrtf(pn_variance),
                                                                               apply_phase_noise);
}

void run_channel_pipeline_cuda_batched(int num_channels,
                                       int nb_tx,
                                       int nb_rx,
                                       int channel_length,
                                       uint32_t num_samples,
                                       void *d_channel_coeffs_batch,
                                       float sigma2,
                                       double ts,
                                       uint16_t pdu_bit_map,
                                       uint16_t ptrs_bit_map,
                                       void *d_tx_sig_batch,
                                       void *d_intermediate_sig_batch,
                                       void *d_final_output_batch,
                                       void *d_curand_states)
{
  float2 *d_tx = (float2 *)d_tx_sig_batch;
  float2 *d_intermediate = (float2 *)d_intermediate_sig_batch;
  short2 *d_final = (short2 *)d_final_output_batch;
  float2 *d_coeffs = (float2 *)d_channel_coeffs_batch;
  curandState_t *d_states = (curandState_t *)d_curand_states;

  dim3 threads_multipath(512, 1, 1);
  dim3 blocks_multipath((num_samples + threads_multipath.x - 1) / threads_multipath.x, nb_rx, num_channels);
  size_t sharedMemSize = (threads_multipath.x + channel_length - 1) * sizeof(float2);

  multipath_channel_kernel_batched<<<blocks_multipath, threads_multipath, sharedMemSize>>>(d_coeffs,
                                                                                           d_tx,
                                                                                           d_intermediate,
                                                                                           num_samples,
                                                                                           channel_length,
                                                                                           nb_tx,
                                                                                           nb_rx);

  dim3 threads_noise(256, 1, 1);
  dim3 blocks_noise((num_samples + threads_noise.x - 1) / threads_noise.x, nb_rx, num_channels);
  float pn_variance = 1e-5f * 2.0f * 3.1415926535f * 300.0f * (float)ts;
  bool apply_phase_noise = (pdu_bit_map & ptrs_bit_map);
  add_noise_and_phase_noise_kernel_batched<<<blocks_noise, threads_noise>>>(d_intermediate,
                                                                            d_final,
                                                                            d_states,
                                                                            num_samples,
                                                                            nb_rx,
                                                                            sqrtf(sigma2 / 2.0f),
                                                                            sqrtf(pn_variance),
                                                                            apply_phase_noise);

  // Synchronization happens in the benchmark (test_channel_scalability) after this call returns.
}
} // extern "C"
