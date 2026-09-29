/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef RU_MPLANE_API_H
#define RU_MPLANE_API_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "common/utils/LOG/log.h"

#define MP_LOG_I(x, args...) LOG_I(HW, "[MPLANE] " x, ##args)
#define MP_LOG_W(x, args...) LOG_W(HW, "[MPLANE] " x, ##args)

#define OPER_STATE \
        X(ENABLED_OPER, "enabled") \
        X(DISABLED_OPER, "disabled")

typedef enum {
#define X(name, str) name,
        OPER_STATE
#undef X
        OPER_COUNT
} oper_state_e;

oper_state_e str_to_enum_oper(const char* value);

#define ADMIN_STATE \
        X(UNLOCKED_ADMIN, "unlocked") \
        X(SHUTTING_DOWN_ADMIN, "shutting-down") \
        X(LOCKED_ADMIN, "locked")

typedef enum {
#define X(name, str) name,
        ADMIN_STATE
#undef X
        ADMIN_COUNT
} admin_state_e;

admin_state_e str_to_enum_admin(const char* value);

#define AVAIL_STATE \
        X(NORMAL_AVAIL, "NORMAL") \
        X(DEGRADED_AVAIL, "DEGRADED") \
        X(FAULTY_AVAIL, "FAULTY")

typedef enum {
#define X(name, str) name,
        AVAIL_STATE
#undef X
        AVAIL_COUNT
} avail_state_e;

avail_state_e str_to_enum_avail(const char* value);

typedef struct {
  oper_state_e oper_state;  //  "enabled", "disabled"
  admin_state_e admin_state; // "unlocked", "shutting-down", "locked"
  avail_state_e avail_state; // "NORMAL", "DEGRADED", "FAULTY"
} hardware_notif_t;

#define PTP_STATE \
        X(LOCKED_PTP,  "LOCKED") \
        X(FREERUN_PTP, "FREERUN") \
        X(HOLDOVER_PTP, "HOLDOVER")

typedef enum {
#define X(name, str) name,
        PTP_STATE
#undef X
        PTP_COUNT
} ptp_state_e;

ptp_state_e str_to_enum_ptp(const char* value);

#define CARRIER_STATE \
        X(READY_CARRIER, "READY") \
        X(DISABLED_CARRIER, "DISABLED") \
        X(BUSY_CARRIER, "BUSY")

typedef enum {
#define X(name, str) name,
        CARRIER_STATE
#undef X
        CARRIER_COUNT
} carrier_state_e;

carrier_state_e str_to_enum_carrier(const char* value);

typedef struct {
  hardware_notif_t hardware;
  ptp_state_e ptp_state; //  "LOCKED", "FREERUN", "HOLDOVER"
  carrier_state_e rx_carrier_state; // "READY", "DISABLED", "BUSY"
  carrier_state_e tx_carrier_state; // "READY", "DISABLED", "BUSY"
  bool config_change;
  // to be extended with any notification callback

} ru_notif_t;

typedef struct {
  char *ru_mac_addr;
  uint32_t mtu;
  uint8_t comp_hdr_type;
  uint8_t iq_width;
  uint8_t prach_offset;

  // DU sends to RU and xran
  uint16_t du_port_bitmask;
  uint16_t band_sector_bitmask;
  uint16_t ccid_bitmask;
  uint16_t ru_port_bitmask;

  // DU retrieves from RU, and sends to xran
  uint8_t du_port;
  uint8_t band_sector;
  uint8_t ccid;
  uint8_t ru_port;
  int16_t frame_str; // might be needed in the future xran releases
  bool managed_delay;

  double max_tx_gain;

} xran_mplane_t;

typedef struct {
  size_t num;
  char **name;
} uplane_info_t;

typedef struct {
  size_t num_cu_planes;
  char **du_mac_addr; // one or two VF(s) for CU-planes
  int32_t *vlan_tag;  // one or two VF(s) for CU-planes
  char *interface_name;
  uplane_info_t tx_endpoints;
  uplane_info_t rx_endpoints;
  uplane_info_t tx_carriers;
  uplane_info_t rx_carriers;

} ru_mplane_config_t;

typedef struct {
  // activation if required at start-up timing
  bool start_up_timing;

  size_t rx_num;
  char **rx_window_meas;

  size_t tx_num;
  char **tx_meas;

} pm_stats_t;

typedef struct {
  char *username;
  char *ru_ip_add;
  ru_mplane_config_t ru_mplane_config;
  void *session;
  void *ctx;
  xran_mplane_t xran_mplane;
  ru_notif_t ru_notif;
  pm_stats_t pm_stats;

} ru_session_t;

typedef struct {
  size_t num_rus;
  ru_session_t *ru_session;
  char **du_key_pair;

} ru_session_list_t;

void free_ru_session_list(ru_session_list_t *src);

bool get_config_for_xran(const char *buffer, const int max_num_ant, xran_mplane_t *xran_mplane);

bool get_uplane_info(const char *buffer, ru_mplane_config_t *ru_mplane_config);

bool get_pm_object_list(const char *buffer, pm_stats_t *pm_stats);

#endif /* RU_MPLANE_API_H */
