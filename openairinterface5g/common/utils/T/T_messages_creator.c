/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Helper functions for creating T-Tracer UL PHY trace messages, it is used by data recording application
 */

/*
 * Each function logs one UL PHY trace message via the T() macro.
 * All messages share the same field format (Data Collection Trace Messages Structure):
 *   int,frame : int,slot :
 *   int,frame_type : int,freq_range : int,subcarrier_spacing : int,cyclic_prefix : int,symbols_per_slot :
 *   int,Nid_cell : int,rnti :
 *   int,rb_size : int,rb_start : int,start_symbol_index : int,nr_of_symbols :
 *   int,qam_mod_order : int,mcs_index : int,mcs_table : int,nrOfLayers :
 *   int,transform_precoding : int,dmrs_config_type : int,ul_dmrs_symb_pos : int,number_dmrs_symbols : int,dmrs_port :
 *   int,dmrs_nscid : int,nb_antennas_rx : int,number_of_bits : buffer,data
 */

#include "T.h"
#include "T_messages_creator.h"

#if T_TRACER

// Internal helper: emit the common UL metadata + buffer via T()
// Note: The T() macro internally checks T_ACTIVE(trace_id) before sending,
// so callers do not need an additional T_ACTIVE() guard.
static inline void log_ul_common(int trace_id,
                                 int frame,
                                 int slot,
                                 const NR_DL_FRAME_PARMS *frame_parms,
                                 const nfapi_nr_pusch_pdu_t *rel15_ul,
                                 int number_dmrs_symbols,
                                 int dmrs_port,
                                 int number_of_bits,
                                 const void *data,
                                 int data_len)
{
  T(trace_id,
    T_INT((int)frame),
    T_INT((int)slot),
    T_INT((int)frame_parms->frame_type),
    T_INT((int)frame_parms->freq_range),
    T_INT((int)rel15_ul->subcarrier_spacing),
    T_INT((int)rel15_ul->cyclic_prefix),
    T_INT((int)frame_parms->symbols_per_slot),
    T_INT((int)frame_parms->Nid_cell),
    T_INT((int)rel15_ul->rnti),
    T_INT((int)rel15_ul->rb_size),
    T_INT((int)rel15_ul->rb_start),
    T_INT((int)rel15_ul->start_symbol_index),
    T_INT((int)rel15_ul->nr_of_symbols),
    T_INT((int)rel15_ul->qam_mod_order),
    T_INT((int)rel15_ul->mcs_index),
    T_INT((int)rel15_ul->mcs_table),
    T_INT((int)rel15_ul->nrOfLayers),
    T_INT((int)rel15_ul->transform_precoding),
    T_INT((int)rel15_ul->dmrs_config_type),
    T_INT((int)rel15_ul->ul_dmrs_symb_pos),
    T_INT((int)number_dmrs_symbols),
    T_INT((int)dmrs_port),
    T_INT((int)rel15_ul->scid),
    T_INT((int)frame_parms->nb_antennas_rx),
    T_INT((int)number_of_bits),
    T_BUFFER(data, data_len));
}

void log_ul_fd_dmrs(int frame,
                    int slot,
                    const NR_DL_FRAME_PARMS *frame_parms,
                    const nfapi_nr_pusch_pdu_t *rel15_ul,
                    int number_dmrs_symbols,
                    int dmrs_port,
                    const c16_t *data,
                    int data_len)
{
  log_ul_common(T_GNB_PHY_UL_FD_DMRS,
                frame, slot, frame_parms, rel15_ul,
                number_dmrs_symbols, dmrs_port, 0, data, data_len);
}

void log_ul_fd_chan_est_dmrs_pos(int frame,
                                int slot,
                                const NR_DL_FRAME_PARMS *frame_parms,
                                const nfapi_nr_pusch_pdu_t *rel15_ul,
                                int number_dmrs_symbols,
                                int dmrs_port,
                                const c16_t *data,
                                int data_len)
{
  log_ul_common(T_GNB_PHY_UL_FD_CHAN_EST_DMRS_POS,
                frame, slot, frame_parms, rel15_ul,
                number_dmrs_symbols, dmrs_port, 0, data, data_len);
}

void log_ul_fd_pusch_iq(int frame,
                        int slot,
                        const NR_DL_FRAME_PARMS *frame_parms,
                        const nfapi_nr_pusch_pdu_t *rel15_ul,
                        int number_dmrs_symbols,
                        int dmrs_port,
                        const c16_t *data,
                        int data_len)
{
  log_ul_common(T_GNB_PHY_UL_FD_PUSCH_IQ,
                frame, slot, frame_parms, rel15_ul,
                number_dmrs_symbols, dmrs_port, 0, data, data_len);
}

void log_ul_fd_chan_est_dmrs_interpl(int frame,
                                    int slot,
                                    const NR_DL_FRAME_PARMS *frame_parms,
                                    const nfapi_nr_pusch_pdu_t *rel15_ul,
                                    int number_dmrs_symbols,
                                    int dmrs_port,
                                    const c16_t *data,
                                    int data_len)
{
  log_ul_common(T_GNB_PHY_UL_FD_CHAN_EST_DMRS_INTERPL,
                frame, slot, frame_parms, rel15_ul,
                number_dmrs_symbols, dmrs_port, 0, data, data_len);
}

void log_ul_payload_rx_bits(int frame,
                           int slot,
                           const NR_DL_FRAME_PARMS *frame_parms,
                           const nfapi_nr_pusch_pdu_t *rel15_ul,
                           int number_dmrs_symbols,
                           int dmrs_port,
                           const uint8_t *data,
                           int tb_size)
{
  log_ul_common(T_GNB_PHY_UL_PAYLOAD_RX_BITS,
                frame, slot, frame_parms, rel15_ul,
                number_dmrs_symbols, dmrs_port,
                tb_size << 3, data, tb_size);
}

// UE-side: uses nfapi_nr_ue_pusch_pdu_t, derives subcarrier_spacing index
// from frame_parms, and logs nb_antennas_tx instead of nb_antennas_rx.
void log_ul_payload_tx_bits(int frame,
                           int slot,
                           const NR_DL_FRAME_PARMS *frame_parms,
                           const nfapi_nr_ue_pusch_pdu_t *pusch_pdu,
                           int number_dmrs_symbols,
                           int dmrs_port,
                           const uint8_t *data,
                           int tb_size)
{
  int subcarrier_spacing_index = frame_parms->subcarrier_spacing / 15000 - 1;
  T(T_UE_PHY_UL_PAYLOAD_TX_BITS,
    T_INT((int)frame),
    T_INT((int)slot),
    T_INT((int)frame_parms->frame_type),
    T_INT((int)frame_parms->freq_range),
    T_INT((int)subcarrier_spacing_index),
    T_INT((int)pusch_pdu->cyclic_prefix),
    T_INT((int)frame_parms->symbols_per_slot),
    T_INT((int)frame_parms->Nid_cell),
    T_INT((int)pusch_pdu->rnti),
    T_INT((int)pusch_pdu->rb_size),
    T_INT((int)pusch_pdu->rb_start),
    T_INT((int)pusch_pdu->start_symbol_index),
    T_INT((int)pusch_pdu->nr_of_symbols),
    T_INT((int)pusch_pdu->qam_mod_order),
    T_INT((int)pusch_pdu->mcs_index),
    T_INT((int)pusch_pdu->mcs_table),
    T_INT((int)pusch_pdu->nrOfLayers),
    T_INT((int)pusch_pdu->transform_precoding),
    T_INT((int)pusch_pdu->dmrs_config_type),
    T_INT((int)pusch_pdu->ul_dmrs_symb_pos),
    T_INT((int)number_dmrs_symbols),
    T_INT((int)dmrs_port),
    T_INT((int)pusch_pdu->scid),
    T_INT((int)frame_parms->nb_antennas_tx),
    T_INT((int)(tb_size << 3)),
    T_BUFFER(data, tb_size));
}

void log_ul_scrambled_tx_bits(int frame,
                             int slot,
                             const NR_DL_FRAME_PARMS *frame_parms,
                             const nfapi_nr_ue_pusch_pdu_t *pusch_pdu,
                             int number_dmrs_symbols,
                             int dmrs_port,
                             const uint8_t *data,
                             int number_of_bits)
{
  int subcarrier_spacing_index = frame_parms->subcarrier_spacing / 15000 - 1;
  T(T_UE_PHY_UL_SCRAMBLED_TX_BITS,
    T_INT((int)frame),
    T_INT((int)slot),
    T_INT((int)frame_parms->frame_type),
    T_INT((int)frame_parms->freq_range),
    T_INT((int)subcarrier_spacing_index),
    T_INT((int)pusch_pdu->cyclic_prefix),
    T_INT((int)frame_parms->symbols_per_slot),
    T_INT((int)frame_parms->Nid_cell),
    T_INT((int)pusch_pdu->rnti),
    T_INT((int)pusch_pdu->rb_size),
    T_INT((int)pusch_pdu->rb_start),
    T_INT((int)pusch_pdu->start_symbol_index),
    T_INT((int)pusch_pdu->nr_of_symbols),
    T_INT((int)pusch_pdu->qam_mod_order),
    T_INT((int)pusch_pdu->mcs_index),
    T_INT((int)pusch_pdu->mcs_table),
    T_INT((int)pusch_pdu->nrOfLayers),
    T_INT((int)pusch_pdu->transform_precoding),
    T_INT((int)pusch_pdu->dmrs_config_type),
    T_INT((int)pusch_pdu->ul_dmrs_symb_pos),
    T_INT((int)number_dmrs_symbols),
    T_INT((int)dmrs_port),
    T_INT((int)pusch_pdu->scid),
    T_INT((int)frame_parms->nb_antennas_tx),
    T_INT((int)number_of_bits),
    T_BUFFER(data, number_of_bits / 8));
}

#endif /* T_TRACER */
