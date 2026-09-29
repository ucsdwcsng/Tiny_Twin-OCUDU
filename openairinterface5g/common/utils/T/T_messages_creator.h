/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Helper functions for creating T-Tracer UL PHY trace messages, it is used by data recording application
 */

#ifndef T_MESSAGES_CREATOR_H
#define T_MESSAGES_CREATOR_H

#if T_TRACER

#include <stdint.h>
#include "common/platform_types.h"
#include "common/utils/nr/nr_common.h"
#include "PHY/defs_nr_common.h"
#include "nfapi_nr_interface_scf.h"
#include "fapi_nr_ue_interface.h"

/**
 * @brief Log PUSCH UL DMRS using T-Tracer
 *
 * This function creates and sends a T_GNB_PHY_UL_FD_DMRS message containing
 * PUSCH DMRS symbols in frequency domain.
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param rel15_ul Pointer to PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to DMRS data buffer (c16_t complex samples)
 * @param data_len Size of the data buffer in bytes
 */
void log_ul_fd_dmrs(int frame,
                    int slot,
                    const NR_DL_FRAME_PARMS *frame_parms,
                    const nfapi_nr_pusch_pdu_t *rel15_ul,
                    int number_dmrs_symbols,
                    int dmrs_port,
                    const c16_t *data,
                    int data_len);

/**
 * @brief Log PUSCH UL channel estimates at DMRS positions using T-Tracer
 *
 * This function creates and sends a T_GNB_PHY_UL_FD_CHAN_EST_DMRS_POS message
 * containing channel estimates at DMRS positions in frequency domain.
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param rel15_ul Pointer to PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to channel estimate data buffer (c16_t complex samples)
 * @param data_len Size of the data buffer in bytes
 */
void log_ul_fd_chan_est_dmrs_pos(int frame,
                                int slot,
                                const NR_DL_FRAME_PARMS *frame_parms,
                                const nfapi_nr_pusch_pdu_t *rel15_ul,
                                int number_dmrs_symbols,
                                int dmrs_port,
                                const c16_t *data,
                                int data_len);

/**
 * @brief Log PUSCH UL IQ data using T-Tracer
 *
 * This function creates and sends a T_GNB_PHY_UL_FD_PUSCH_IQ message
 * containing PUSCH IQ samples in frequency domain.
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param rel15_ul Pointer to PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to PUSCH IQ data buffer (c16_t complex samples)
 * @param data_len Size of the data buffer in bytes
 */
void log_ul_fd_pusch_iq(int frame,
                        int slot,
                        const NR_DL_FRAME_PARMS *frame_parms,
                        const nfapi_nr_pusch_pdu_t *rel15_ul,
                        int number_dmrs_symbols,
                        int dmrs_port,
                        const c16_t *data,
                        int data_len);

/**
 * @brief Log PUSCH UL interpolated channel estimates using T-Tracer
 *
 * This function creates and sends a T_GNB_PHY_UL_FD_CHAN_EST_DMRS_INTERPL message
 * containing interpolated channel estimates across all subcarriers in frequency domain.
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param rel15_ul Pointer to PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to interpolated channel estimate data buffer (c16_t complex samples)
 * @param data_len Size of the data buffer in bytes
 */
void log_ul_fd_chan_est_dmrs_interpl(int frame,
                                    int slot,
                                    const NR_DL_FRAME_PARMS *frame_parms,
                                    const nfapi_nr_pusch_pdu_t *rel15_ul,
                                    int number_dmrs_symbols,
                                    int dmrs_port,
                                    const c16_t *data,
                                    int data_len);

/**
 * @brief Log PUSCH UL received payload bits using T-Tracer
 *
 * This function creates and sends a T_GNB_PHY_UL_PAYLOAD_RX_BITS message
 * containing the decoded transport block payload data.
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param rel15_ul Pointer to PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to decoded transport block data (uint8_t bytes)
 * @param tb_size Transport block size in bytes
 */
void log_ul_payload_rx_bits(int frame,
                           int slot,
                           const NR_DL_FRAME_PARMS *frame_parms,
                           const nfapi_nr_pusch_pdu_t *rel15_ul,
                           int number_dmrs_symbols,
                           int dmrs_port,
                           const uint8_t *data,
                           int tb_size);

/**
 * @brief Log UE PUSCH UL transmitted payload bits using T-Tracer
 *
 * This function creates and sends a T_UE_PHY_UL_PAYLOAD_TX_BITS message
 * containing the transport block payload data before encoding.
 * Note: subcarrier_spacing is derived from frame_parms->subcarrier_spacing
 * and nb_antennas_tx is used (UE transmit side).
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param pusch_pdu Pointer to UE PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to transport block payload data (uint8_t bytes)
 * @param tb_size Transport block size in bytes
 */
void log_ul_payload_tx_bits(int frame,
                           int slot,
                           const NR_DL_FRAME_PARMS *frame_parms,
                           const nfapi_nr_ue_pusch_pdu_t *pusch_pdu,
                           int number_dmrs_symbols,
                           int dmrs_port,
                           const uint8_t *data,
                           int tb_size);

/**
 * @brief Log UE PUSCH UL scrambled transmit bits using T-Tracer
 *
 * This function creates and sends a T_UE_PHY_UL_SCRAMBLED_TX_BITS message
 * containing the scrambled codeword bits after scrambling.
 * Note: subcarrier_spacing is derived from frame_parms->subcarrier_spacing
 * and nb_antennas_tx is used (UE transmit side).
 *
 * @param frame Frame number
 * @param slot Slot number
 * @param frame_parms Pointer to frame parameters structure
 * @param pusch_pdu Pointer to UE PUSCH PDU structure
 * @param number_dmrs_symbols Number of DMRS symbols
 * @param dmrs_port DMRS port number
 * @param data Pointer to scrambled codeword data (uint8_t bytes)
 * @param number_of_bits Number of scrambled bits
 */
void log_ul_scrambled_tx_bits(int frame,
                             int slot,
                             const NR_DL_FRAME_PARMS *frame_parms,
                             const nfapi_nr_ue_pusch_pdu_t *pusch_pdu,
                             int number_dmrs_symbols,
                             int dmrs_port,
                             const uint8_t *data,
                             int number_of_bits);

#endif /* T_TRACER */

#endif /* T_MESSAGES_CREATOR_H */
