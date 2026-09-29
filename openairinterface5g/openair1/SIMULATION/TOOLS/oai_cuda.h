/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __OAI_CUDA_H__
#define __OAI_CUDA_H__

#include <stdint.h>

#ifdef __NVCC__
#include "common/platform_types.h"
#else
#include "PHY/TOOLS/tools_defs.h"
#endif

#ifdef __NVCC__
#include <curand_kernel.h>
__device__ float2 complex_mul(float2 a, float2 b);

__global__ void multipath_channel_kernel(const float2 *__restrict__ d_channel_coeffs,
                                         const float *__restrict__ tx_sig,
                                         float2 *__restrict__ rx_sig,
                                         int num_samples,
                                         int channel_length,
                                         int nb_tx,
                                         int nb_rx);

__global__ void add_noise_and_phase_noise_kernel(const float2 *__restrict__ r_sig,
                                                 short2 *__restrict__ output_sig,
                                                 curandState_t *states,
                                                 int num_samples,
                                                 float sigma,
                                                 float pn_std_dev,
                                                 bool apply_phase_noise);

#endif // __NVCC__

#ifdef __cplusplus
extern "C" {
#endif

void init_cuda_chsim_buffers(int use_cuda,
                             int n_tx,
                             int n_rx,
                             void **d_tx_sig_p,
                             void **d_intermediate_sig_p,
                             void **d_final_output_p,
                             void **d_curand_states_p,
                             void **h_tx_sig_pinned_p,
                             void **h_final_output_pinned_p,
                             void **d_channel_coeffs_gpu_p);

void free_cuda_chsim_buffers(int use_cuda,
                             void **d_tx_sig_p,
                             void **d_intermediate_sig_p,
                             void **d_final_output_p,
                             void **d_curand_states_p,
                             void **h_tx_sig_pinned_p,
                             void **h_final_output_pinned_p,
                             float **h_channel_coeffs_p,
                             void **d_channel_coeffs_gpu_p);

void multipath_channel_cuda(float **rx_sig_re,
                            float **rx_sig_im,
                            int nb_tx,
                            int nb_rx,
                            int channel_length,
                            uint32_t length,
                            uint64_t channel_offset,
                            float *h_channel_coeffs,
                            void *d_tx_sig,
                            void *d_rx_sig,
                            void *d_channel_coeffs,
                            void *h_tx_sig_pinned);

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
                    void *d_r_sig,
                    void *d_output_sig,
                    void *d_curand_states,
                    void *h_r_sig_pinned,
                    void *h_output_sig_pinned);

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
                               void *d_tx_sig,
                               void *d_intermediate_sig,
                               void *d_final_output,
                               void *d_curand_states,
                               void *h_tx_sig_pinned,
                               void *h_final_output_pinned,
                               void *d_channel_coeffs);

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
                                       void *d_curand_states);

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
                                        void *stream_void);

void sum_channel_outputs_cuda(void **d_individual_outputs, void *d_final_output, int num_channels, int nb_rx, int num_samples);

/**
 * \brief Interleaves separate real and imaginary signal buffers into a complex format.
 *
 * This utility function takes separate planar buffers for the real (I) and imaginary (Q)
 * components of a signal and combines them into a single interleaved buffer of float2
 * structs, which is a common format for CUDA processing.
 *
 * \param[in]  rx_sig_re          Array of pointers to the real parts of the input signals.
 * \param[in]  rx_sig_im          Array of pointers to the imaginary parts of the input signals.
 * \param[out] output_interleaved Pointer to an array of pointers where the interleaved output will be stored.
 * \param[in]  nb_rx              The number of receive antennas (i.e., the number of signals).
 * \param[in]  num_samples        The number of samples per signal.
 *
 * \note The caller is responsible for allocating memory for the `output_interleaved` buffer. It should
 * point to an array of `nb_rx` pointers, where each of those pointers is allocated with a size of
 * `num_samples * sizeof(float2)`.
 */
void interleave_channel_output_cuda(float **rx_sig_re, float **rx_sig_im, void **output_interleaved, int nb_rx, int num_samples);

void *create_and_init_curand_states_cuda(int num_elements, unsigned long long seed);
void destroy_curand_states_cuda(void *d_curand_states);

#ifdef __cplusplus
}
#endif

#endif // __OAI_CUDA_H__
