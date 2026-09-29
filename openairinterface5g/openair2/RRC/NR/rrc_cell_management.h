/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef RRC_CELL_MANAGEMENT_H_
#define RRC_CELL_MANAGEMENT_H_

#include "nr_rrc_defs.h" // for nr_rrc_du_container_t, nr_rrc_cell_container_t, RB_HEAD
#include "common/utils/collection/tree.h"
#include <stdbool.h>
#include <stdint.h>

/** @brief ServCellIndex value for Primary Cell (TS 38.331)
 * @note ServCellIndex is a short identity used to uniquely identify a serving cell
 *       (PCell, PSCell, or SCell) across cell groups. Value 0 applies for the PCell. */
#define RRC_PCELL_INDEX 0

/** @brief Maximum number of serving cells (TS 38.331)
 * @note maxNrofServingCells = 32 (1 PCell + up to 31 SCells)
 * @note ServCellIndex range: INTEGER (0..maxNrofServingCells-1) = 0..31 */
#define RRC_MAX_NUM_SERVING_CELLS 32

// Generate RB tree functions (prototypes)
int rrc_cell_cmp(const struct nr_rrc_cell_container_t *a, const struct nr_rrc_cell_container_t *b);
RB_PROTOTYPE(rrc_cell_tree, nr_rrc_cell_container_t, entries, rrc_cell_cmp);
int du_compare(const struct nr_rrc_du_container_t *a, const struct nr_rrc_du_container_t *b);
RB_PROTOTYPE(rrc_du_tree, nr_rrc_du_container_t, entries, du_compare);

// DU management (global DU tree)
nr_rrc_du_container_t *rrc_add_du(gNB_RRC_INST *rrc, nr_rrc_du_container_t *du);
void rrc_rm_du(gNB_RRC_INST *rrc, nr_rrc_du_container_t *du);
void rrc_cleanup_du(gNB_RRC_INST *rrc, nr_rrc_du_container_t *du);
void rrc_free_du_container(nr_rrc_du_container_t *du);
nr_rrc_du_container_t *get_du_by_assoc_id(gNB_RRC_INST *rrc, const sctp_assoc_t assoc_id);

// Cell management (global cell tree)
nr_rrc_cell_container_t *get_cell_by_cell_id(struct rrc_cell_tree *cells, const uint64_t cell_id);
nr_rrc_cell_container_t *rrc_add_cell(gNB_RRC_INST *rrc, nr_rrc_cell_container_t *cell);
void rrc_rm_cell(gNB_RRC_INST *rrc, nr_rrc_cell_container_t *cell);

// Cell management (DU-specific cell tree)
nr_rrc_cell_container_t *rrc_get_cell_for_du(seq_arr_t *cells, uint64_t cell_id);
nr_rrc_cell_container_t *rrc_get_cell_by_pci_for_du(const seq_arr_t *cells, uint16_t pci);
nr_rrc_cell_container_t *rrc_add_cell_to_du(seq_arr_t *cells, nr_rrc_cell_container_t *cell);

// Cell cleanup
void rrc_free_cell_container(nr_rrc_cell_container_t *cell);

// UE serving cell functions
ue_serving_cell_t *rrc_add_ue_serving_cell(gNB_RRC_UE_t *ue, const nr_rrc_cell_container_t *cell, uint8_t serving_cell_id);
void rrc_remove_ue_scells_from_du(gNB_RRC_UE_t *ue, sctp_assoc_t assoc_id);
ue_serving_cell_t *rrc_get_ue_serving_cell_by_id(const gNB_RRC_UE_t *ue, uint8_t serving_cell_id);
const ue_serving_cell_t *ue_get_pcell_entry(const gNB_RRC_UE_t *ue);
nr_rrc_cell_container_t *rrc_get_pcell_for_ue(gNB_RRC_INST *rrc, const gNB_RRC_UE_t *ue);
ue_serving_cell_t *rrc_update_ue_pcell(gNB_RRC_UE_t *UE, const nr_rrc_cell_container_t *cell);

#endif /* RRC_CELL_MANAGEMENT_H_ */
