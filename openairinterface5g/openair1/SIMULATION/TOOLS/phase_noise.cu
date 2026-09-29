/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include "oai_cuda.h"

#define CHECK_CUDA(val) checkCuda((val), __FILE__, __LINE__)
static void checkCuda(cudaError_t result, const char *file, int line)
{
  if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Error at %s:%d: %s\n", file, line, cudaGetErrorString(result));
    exit(1);
  }
}

__device__ __forceinline__ float2 complex_mul(float2 a, float2 b)
{
  return make_float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

__global__ void init_curand_states_kernel(curandState_t *states, unsigned long long seed, int num_elements)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < num_elements) {
    curand_init(seed, idx, 0, &states[idx]);
  }
}

__global__ void add_noise_and_phase_noise_kernel(const float2 *__restrict__ r_sig,
                                                 short2 *__restrict__ output_sig,
                                                 curandState_t *states,
                                                 int num_samples,
                                                 float sigma,
                                                 float pn_std_dev,
                                                 bool apply_phase_noise)
{
  const int i = blockIdx.x * blockDim.x + threadIdx.x;
  const int ii = blockIdx.y;

  if (i >= num_samples)
    return;

  // Each thread handles one sample and its corresponding cuRAND state
  curandState_t local_state = states[ii * num_samples + i];

  float2 noisy_signal = r_sig[ii * num_samples + i];

  float2 awgn = curand_normal2(&local_state);
  noisy_signal.x += awgn.x * sigma;
  noisy_signal.y += awgn.y * sigma;

  float2 final_signal = noisy_signal;

  if (apply_phase_noise) {
    float phase_error = curand_normal(&local_state) * pn_std_dev;
    float cos_phi, sin_phi;
    __sincosf(phase_error, &sin_phi, &cos_phi);
    float2 phase_rot = make_float2(cos_phi, sin_phi);
    final_signal = complex_mul(noisy_signal, phase_rot);
  }

  states[ii * num_samples + i] = local_state;
  output_sig[ii * num_samples + i] = make_short2((short)fmaxf(-32768.0f, fminf(32767.0f, final_signal.x)),
                                                 (short)fmaxf(-32768.0f, fminf(32767.0f, final_signal.y)));
}

__global__ void add_noise_and_phase_noise_kernel_batched(const float2 *__restrict__ r_sig,
                                                         short2 *__restrict__ output_sig,
                                                         curandState_t *states,
                                                         int num_samples,
                                                         int nb_rx,
                                                         float sigma,
                                                         float pn_std_dev,
                                                         bool apply_phase_noise)
{
  const int i = blockIdx.x * blockDim.x + threadIdx.x; // Sample index
  const int ii = blockIdx.y; // RX antenna index
  const int c = blockIdx.z; // Channel index

  if (i >= num_samples)
    return;

  const int flat_output_idx = (c * nb_rx * num_samples) + (ii * num_samples) + i;
  // Index for the smaller, reused pool of cuRAND states (does NOT include channel)
  const int state_pool_idx = (ii * num_samples) + i;

  curandState_t local_state = states[state_pool_idx];

  float2 noisy_signal = r_sig[flat_output_idx];

  float2 awgn = curand_normal2(&local_state);
  noisy_signal.x += awgn.x * sigma;
  noisy_signal.y += awgn.y * sigma;

  float2 final_signal = noisy_signal;

  if (apply_phase_noise) {
    float phase_error = curand_normal(&local_state) * pn_std_dev;
    float cos_phi, sin_phi;
    __sincosf(phase_error, &sin_phi, &cos_phi);
    float2 phase_rot = make_float2(cos_phi, sin_phi);
    final_signal = complex_mul(noisy_signal, phase_rot);
  }

  states[state_pool_idx] = local_state;

  output_sig[flat_output_idx] = make_short2((short)fmaxf(-32768.0f, fminf(32767.0f, final_signal.x)),
                                            (short)fmaxf(-32768.0f, fminf(32767.0f, final_signal.y)));
}

extern "C" {

void add_noise_cuda(const float **r_re,
                    const float **r_im,
                    c16_t **output_signal,
                    int num_samples,
                    int nb_rx,
                    float sigma2,
                    double ts,
                    int slot_offset,
                    int delay,
                    uint16_t pdu_bit_map,
                    uint16_t ptrs_bit_map,
                    void *d_r_sig_void,
                    void *d_output_sig_void,
                    void *d_curand_states_void,
                    void *h_r_sig_void,
                    void *h_output_temp_void)
{
  float2 *d_r_sig = (float2 *)d_r_sig_void;
  short2 *d_output_sig = (short2 *)d_output_sig_void;
  curandState_t *d_curand_states = (curandState_t *)d_curand_states_void;

  float2 *kernel_input_ptr;

#if defined(USE_UNIFIED_MEMORY)
  for (int ii = 0; ii < nb_rx; ii++) {
    for (int i = 0; i < num_samples; i++) {
      d_r_sig[ii * num_samples + i] = make_float2(r_re[ii][i], r_im[ii][i]);
    }
  }
  kernel_input_ptr = d_r_sig;
#elif defined(USE_ATS_MEMORY)
  float2 *h_r_sig = (float2 *)h_r_sig_void;
  for (int ii = 0; ii < nb_rx; ii++) {
    for (int i = 0; i < num_samples; i++) {
      h_r_sig[ii * num_samples + i] = make_float2(r_re[ii][i], r_im[ii][i]);
    }
  }
  kernel_input_ptr = h_r_sig;
#else // EXPLICIT COPY
  float2 *h_r_sig = (float2 *)h_r_sig_void;
  for (int ii = 0; ii < nb_rx; ii++) {
    for (int i = 0; i < num_samples; i++) {
      h_r_sig[ii * num_samples + i] = make_float2(r_re[ii][i], r_im[ii][i]);
    }
  }
  CHECK_CUDA(cudaMemcpy(d_r_sig, h_r_sig, nb_rx * num_samples * sizeof(float2), cudaMemcpyHostToDevice));
  kernel_input_ptr = d_r_sig;
#endif

  dim3 threadsPerBlock(256, 1);
  dim3 numBlocks((num_samples + threadsPerBlock.x - 1) / threadsPerBlock.x, nb_rx);
  float pn_variance = 1e-5f * 2.0f * 3.1415926535f * 300.0f * (float)ts;
  bool apply_phase_noise = (pdu_bit_map & ptrs_bit_map);
  add_noise_and_phase_noise_kernel<<<numBlocks, threadsPerBlock>>>(kernel_input_ptr,
                                                                   d_output_sig,
                                                                   d_curand_states,
                                                                   num_samples,
                                                                   sqrtf(sigma2 / 2.0f),
                                                                   sqrtf(pn_variance),
                                                                   apply_phase_noise);

#if defined(USE_UNIFIED_MEMORY)
  CHECK_CUDA(cudaDeviceSynchronize());
  for (int ii = 0; ii < nb_rx; ii++) {
    for (int i = 0; i < num_samples; i++) {
      short2 result = d_output_sig[ii * num_samples + i];
      output_signal[ii][i + slot_offset + delay].r = result.x;
      output_signal[ii][i + slot_offset + delay].i = result.y;
    }
  }
#else
  short2 *h_output_temp = (short2 *)h_output_temp_void;
  CHECK_CUDA(cudaMemcpy(h_output_temp, d_output_sig, nb_rx * num_samples * sizeof(short2), cudaMemcpyDeviceToHost));
  CHECK_CUDA(cudaDeviceSynchronize());
  for (int ii = 0; ii < nb_rx; ii++) {
    memcpy(output_signal[ii] + slot_offset + delay, h_output_temp + ii * num_samples, num_samples * sizeof(short2));
  }
#endif
}

void *create_and_init_curand_states_cuda(int num_elements, unsigned long long seed)
{
  void *d_states_void;
  CHECK_CUDA(cudaMalloc(&d_states_void, num_elements * sizeof(curandState_t)));

  int threads = 256;
  int blocks = (num_elements + threads - 1) / threads;
  init_curand_states_kernel<<<blocks, threads>>>((curandState_t *)d_states_void, seed, num_elements);
  CHECK_CUDA(cudaDeviceSynchronize());

  return d_states_void;
}

void destroy_curand_states_cuda(void *d_curand_states)
{
  if (d_curand_states) {
    CHECK_CUDA(cudaFree(d_curand_states));
  }
}
} // extern "C"
