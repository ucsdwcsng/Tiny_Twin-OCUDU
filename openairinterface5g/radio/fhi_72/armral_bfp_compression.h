/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief BFP compression for the O-RAN 7.2 FrontHaul Interface for Arm systems using the Arm RAN Acceleration Library
 */

#ifndef __ARMRAL_BFP_COMPRESSION_H__
#define __ARMRAL_BFP_COMPRESSION_H__

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
void armral_bfp_compression(uint32_t iq_width, uint32_t n_prb, int16_t *src, int8_t *dst);

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
void armral_bfp_decompression(uint32_t iq_width, uint32_t n_prb, int8_t *src, int16_t *dst);

#endif
