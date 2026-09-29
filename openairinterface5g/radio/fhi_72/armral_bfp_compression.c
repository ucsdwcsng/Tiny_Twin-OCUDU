/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief BFP compression for the O-RAN 7.2 FrontHaul Interface for Arm systems using the Arm RAN Acceleration Library
 */

#include "armral.h"
#include <rte_branch_prediction.h>

#include "armral_bfp_compression.h"
#include "common/utils/LOG/log.h"

/**
 * The function operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 16-bit complex resource elements. Each block
 * taken as input is compressed into 24 samples with a common unsigned scaling factor.
 *
 * @param[in]     iq_width  Width in bits of compressed samples.
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output compressed data.
 */
void armral_bfp_compression(uint32_t iq_width, uint32_t n_prb, int16_t *src, int8_t *dst)
{
  armral_status ret = ARMRAL_ARGUMENT_ERROR;
  switch (iq_width) {
    case 8:
      ret = armral_block_float_compr_8bit(n_prb, (armral_cmplx_int16_t *)src, (armral_compressed_data_8bit *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_compr_8bit returned an error: %d\n", (int)ret);
      }
      break;

    case 9:
      ret = armral_block_float_compr_9bit(n_prb, (armral_cmplx_int16_t *)src, (armral_compressed_data_9bit *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_compr_9bit returned an error: %d\n", (int)ret);
      }
      break;

    case 14:
      ret = armral_block_float_compr_14bit(n_prb, (armral_cmplx_int16_t *)src, (armral_compressed_data_14bit *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_compr_14bit returned an error: %d\n", (int)ret);
      }
      break;

    default:
      LOG_E(HW, "Unsupported IQ width in BFP compression: %d\n", iq_width);
  }
}

/**
 * The function operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 compressed complex resource elements.
 * Each block taken as input is expanded into 12 16-bit complex samples.
 *
 * @param[in]     iq_width  Width in bits of compressed samples.
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input compressed data.
 * @param[out]    dst       Points to the output complex samples sequence.
 */
void armral_bfp_decompression(uint32_t iq_width, uint32_t n_prb, int8_t *src, int16_t *dst)
{
  armral_status ret = ARMRAL_ARGUMENT_ERROR;
  switch (iq_width) {
    case 8:
      ret = armral_block_float_decompr_8bit(n_prb, (armral_compressed_data_8bit *)src, (armral_cmplx_int16_t *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_decompr_8bit returned an error: %d\n", (int)ret);
      }
      break;

    case 9:
      ret = armral_block_float_decompr_9bit(n_prb, (armral_compressed_data_9bit *)src, (armral_cmplx_int16_t *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_decompr_9bit returned an error: %d\n", (int)ret);
      }
      break;

    case 14:
      ret = armral_block_float_decompr_14bit(n_prb, (armral_compressed_data_14bit *)src, (armral_cmplx_int16_t *)dst, NULL);
      if (unlikely(ret != ARMRAL_SUCCESS)) {
        LOG_E(HW, "armral_block_scaling_decompr_14bit returned an error: %d\n", (int)ret);
      }
      break;

    default:
      LOG_E(HW, "Unsupported IQ width in BFP decompression: %d\n", iq_width);
  }
}
