/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int want_model_id; // 0..4 for TDL-A..E, or -1 for any
  int want_tx; // exact match
  int want_rx; // exact match
  float want_ds_ns; // nearest
  float want_speed_mps; // nearest
  float want_aoa_deg; // exact match; 0.0 = broadside default
  int allow_shape_swap; // allow TX/RX swap when matching
  float w_ds; // DS distance weight
  float w_speed; // speed distance weight
  const char *yaml_path; // sidecar path
} cirdb_select_req_t;

typedef struct {
  int model_id;
  int n_tx;
  int n_rx;
  int L; // taps per link in file
  int S; // snapshots
  double fs_hz;
  double snapshot_dt_s;
  float ds_ns;
  float speed_mps;
  int pair_order;
  uint64_t offset_bytes;
  uint64_t nbytes;
} cirdb_entry_meta_t;

int cirdb_yaml_select(const cirdb_select_req_t *req, cirdb_entry_meta_t *out);
int cirdb_yaml_scan(const char *yaml_path);

#ifdef __cplusplus
}
#endif
