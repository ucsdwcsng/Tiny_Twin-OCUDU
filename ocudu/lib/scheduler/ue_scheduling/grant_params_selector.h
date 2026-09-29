// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/cell_harq_manager.h"
#include "../slicing/slice_ue_repository.h"
#include "../support/sch_pdu_builder.h"
#include "ocudu/ran/sch/sch_mcs.h"

namespace ocudu {

class ue_cell;
class slice_ue;

namespace sched_helper {

/// PDCCH and PDSCH parameters recommended for a DL grant.
struct dl_sched_context {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PDSCH time-domain resource index. Indexes the PDSCH TDRA list in use for the SearchSpace (Rel-16 list if
  /// configured, else common/legacy) and is signalled directly in the DCI Time domain resource assignment field.
  uint8_t pdsch_td_res_index;
  /// Recommended MCS, considering channel state or, in case of reTx, last HARQ MCS.
  sch_mcs_index recommended_mcs;
  /// Recommended number of layers.
  unsigned recommended_ri;
  /// Expected number of RBs to allocate.
  unsigned expected_nof_rbs;
  /// Pending bytes for newTx.
  units::bytes pending_bytes;
  /// Number of Rel-16 slot-based PDSCH repetitions of the selected TDRA row, or nullopt for a single transmission.
  /// Equals the \c nof_repetitions requested by the caller when a repetition row was selected.
  std::optional<uint8_t> nof_repetitions;
};

/// Retrieve recommended PDCCH and PDSCH parameters for a newTx DL grant.
/// \param[in] nof_repetitions Requested number of PDSCH repetitions (link-adaptation decision). When set, the selector
/// picks the Rel-16 TDRA row carrying that repetitionNumber-r16 and returns nullopt if it does not fit the slot; when
/// unset, it picks a single-transmission row.
std::optional<dl_sched_context> get_newtx_dl_sched_context(const slice_ue&        u,
                                                           slot_point             pdcch_slot,
                                                           slot_point             pdsch_slot,
                                                           bool                   interleaving_enabled,
                                                           units::bytes           pending_bytes,
                                                           std::optional<uint8_t> nof_repetitions);

/// Retrieve recommended PDCCH and PDSCH parameters for a reTx DL grant.
/// \param[in] nof_repetitions Requested number of PDSCH repetitions (link-adaptation decision). See
/// \ref get_newtx_dl_sched_context.
std::optional<dl_sched_context> get_retx_dl_sched_context(const slice_ue&               u,
                                                          slot_point                    pdcch_slot,
                                                          slot_point                    pdsch_slot,
                                                          bool                          interleaving_enabled,
                                                          const dl_harq_process_handle& h_dl,
                                                          std::optional<uint8_t>        nof_repetitions,
                                                          unsigned                      max_rbs = MAX_NOF_PRBS);

/// Select DL VRBs to allocate for a newTx.
vrb_interval compute_newtx_dl_vrbs(const dl_sched_context& decision_ctxt,
                                   const vrb_bitmap&       used_vrbs,
                                   unsigned                max_nof_rbs = MAX_NOF_PRBS);

/// Select DL VRBs to allocate for a reTx.
vrb_interval compute_retx_dl_vrbs(const dl_sched_context& decision_ctxt, const vrb_bitmap& used_vrbs);

/// PDCCH and PUSCH parameters recommended for a UL grant.
struct ul_sched_context {
  /// SearchSpace to use.
  search_space_id ss_id;
  /// PUSCH time-domain resource index.
  uint8_t pusch_td_res_index;
  /// Limits on VRBs for UL grant allocation.
  vrb_interval vrb_lims;
  /// Limits for grant size in RBs.
  interval<unsigned> nof_rb_lims;
  /// Recommended MCS, considering channel state or, in case of reTx, last HARQ MCS.
  sch_mcs_index recommended_mcs;
  /// Expected number of RBs to allocate.
  unsigned expected_nof_rbs;
  /// Pending bytes for newTx.
  units::bytes pending_bytes;
  /// PUSCH config params.
  pusch_config_params pusch_cfg;
};

/// Retrieve recommended PDCCH and PUSCH parameters for a newTx UL grant.
std::optional<ul_sched_context> get_newtx_ul_sched_context(const slice_ue&   u,
                                                           slot_point        pdcch_slot,
                                                           slot_point        pusch_slot,
                                                           unsigned          uci_nof_harq_bits,
                                                           units::bytes      pending_bytes,
                                                           ofdm_symbol_range allowed_symbols);

/// Retrieve recommended PDCCH and PUSCH parameters for a reTx UL grant.
std::optional<ul_sched_context> get_retx_ul_sched_context(const slice_ue&               u,
                                                          slot_point                    pdcch_slot,
                                                          slot_point                    pusch_slot,
                                                          unsigned                      uci_nof_harq_bits,
                                                          const ul_harq_process_handle& h_ul,
                                                          ofdm_symbol_range             allowed_symbols,
                                                          unsigned                      max_rbs = MAX_NOF_PRBS);

/// Select UL VRBs to allocate for a newTx.
vrb_interval compute_newtx_ul_vrbs(const ul_sched_context& decision_ctxt,
                                   const vrb_bitmap&       used_vrbs,
                                   unsigned                max_nof_rbs = MAX_NOF_PRBS);

/// Select UL VRBs to allocate for a reTx.
vrb_interval compute_retx_ul_vrbs(const ul_sched_context& decision_ctxt, const vrb_bitmap& used_vrbs);

} // namespace sched_helper
} // namespace ocudu
