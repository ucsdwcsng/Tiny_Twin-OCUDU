/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "ru-mplane-api.h"
#include "init-mplane.h"
#include "xml/get-xml.h"
#include "common/utils/assertions.h"

#include <string.h>

#include <libyang/libyang.h>
#include <nc_client.h>

static void free_pm_stats(pm_stats_t *src)
{
  // free Rx window measurements
  for (size_t i = 0; i < src->rx_num; i++) {
    free(src->rx_window_meas[i]);
  }
  free(src->rx_window_meas);

  // free Tx measurements
  for (size_t i = 0; i < src->tx_num; i++) {
    free(src->tx_meas[i]);
  }
  free(src->tx_meas);
}

static void free_uplane_info(uplane_info_t *src)
{
  for (size_t i = 0; i < src->num; i++) {
    free(src->name[i]);
  }
  free(src->name);
}

static void free_ru_mplane_config(ru_mplane_config_t *src)
{
  // free DU MAC address(es) and VLAN tag(s)
  for (size_t i = 0; i < src->num_cu_planes; i++) {
    free(src->du_mac_addr[i]);
  }
  free(src->du_mac_addr);
  free(src->vlan_tag);

  // free interface name
  free(src->interface_name);

  // free endpoints and carriers
  free_uplane_info(&src->tx_endpoints);
  free_uplane_info(&src->rx_endpoints);
  free_uplane_info(&src->tx_carriers);
  free_uplane_info(&src->rx_carriers);
}

static void free_ru_session(ru_session_t *src)
{
  // username
  free(src->username);

  // if no RU connected to, free only the initialized RU IP address
  if (src->session == NULL) {
    free(src->ru_ip_add);
    return;
  }

  // disconnect from the RU
  MP_LOG_I("Sending PM de-activation request for RU \"%s\".\n", src->ru_ip_add);
  bool success = pm_conf(src, "false");
  if (success)
    MP_LOG_I("Successfully de-activated PM for RU \"%s\".\n", src->ru_ip_add);
  MP_LOG_I("Disconnecting from RU \"%s\".\n", src->ru_ip_add);

  // free RU IP address
  free(src->ru_ip_add);
  // free RU M-plane config
  free_ru_mplane_config(&src->ru_mplane_config);
  // free NETCONF session
  nc_session_free(src->session, NULL);
  src->session = NULL;
  // free libyang context
#ifdef MPLANE_V1
  ly_ctx_destroy((struct ly_ctx *)src->ctx, NULL);
#elif defined MPLANE_V2
  ly_ctx_destroy((struct ly_ctx *)src->ctx);
#endif
  // free only the RU MAC addressin xran M-plane info
  free(src->xran_mplane.ru_mac_addr);
  // nothing to free in ru_notif
  // free PM stats
  free_pm_stats(&src->pm_stats);
}

void free_ru_session_list(ru_session_list_t *src)
{
  // DU key pair
  for (size_t i = 0; i < 2; i++) {
    free(src->du_key_pair[i]);
  }
  free(src->du_key_pair);

  // RU session
  for (size_t i = 0; i < src->num_rus; i++) {
    free_ru_session(&src->ru_session[i]);
  }
  free(src->ru_session);
}

oper_state_e str_to_enum_oper(const char* value) {
#define X(name, str) if (value != NULL && strcmp(value, str) == 0) return name;
    OPER_STATE
#undef X
    return OPER_COUNT;
};

admin_state_e str_to_enum_admin(const char* value) {
#define X(name, str) if (value != NULL && strcmp(value, str) == 0) return name;
    ADMIN_STATE
#undef X
    return ADMIN_COUNT;
};

avail_state_e str_to_enum_avail(const char* value) {
#define X(name, str) if (value != NULL && strcmp(value, str) == 0) return name;
    AVAIL_STATE
#undef X
    return AVAIL_COUNT;
};

ptp_state_e str_to_enum_ptp(const char* value) {
#define X(name, str) if (value != NULL && strcmp(value, str) == 0) return name;
    PTP_STATE
#undef X
    return PTP_COUNT;
};

carrier_state_e str_to_enum_carrier(const char* value) {
#define X(name, str) if (value != NULL && strcmp(value, str) == 0) return name;
    CARRIER_STATE
#undef X
    return CARRIER_COUNT;
};

static void free_match_list(char **match_list, size_t count)
{
  for (size_t i = 0; i < count; i++) {
    free(match_list[i]);
  }
  free(match_list);
}

static void fix_benetel_setting(xran_mplane_t *xran_mplane, const uint32_t interface_mtu, const int16_t first_iq_width, const int max_num_ant, const char *model_name)
{
  if (interface_mtu == 1500) {
    MP_LOG_I("Interface MTU %d unreliable/not correctly reported by Benetel O-RU, hardcoding to 9216.\n", interface_mtu);
    xran_mplane->mtu = 9216;
  } else {
    xran_mplane->mtu = interface_mtu;
  }

  if (xran_mplane->iq_width == 255) {
    if (first_iq_width != 9) {
      MP_LOG_I("IQ bitwidth %d unreliable/not correctly reported by Benetel O-RU, hardcoding to 9.\n", first_iq_width);
      xran_mplane->iq_width = 9;
    } else {
      xran_mplane->iq_width = first_iq_width;
    }
  }

  xran_mplane->prach_offset = (max_num_ant == 2) ? 8 : max_num_ant;

  if (strcasecmp(model_name, "RAN550") == 0) {
    xran_mplane->max_tx_gain = 24.0;
  } else if (strcasecmp(model_name, "RAN650") == 0) {
    xran_mplane->max_tx_gain = 35.0;
  } else {
    assert(false && "[MPLANE] Unknown Benetel model name.\n");
  }
}

bool get_config_for_xran(const char *buffer, const int max_num_ant, xran_mplane_t *xran_mplane)
{
  /* some O-RU vendors are not fully compliant as per M-plane specifications */
  char *ru_vendor = get_ru_xml_node(buffer, "mfg-name");

  // RU MAC
  xran_mplane->ru_mac_addr = get_ru_xml_node(buffer, "mac-address"); // TODO: support for VVDN, as it defines multiple MAC addresses

  // MTU
  char *int_mtu_str = get_ru_xml_node(buffer, "l2-mtu");
  const uint32_t interface_mtu = (uint32_t)atoi(int_mtu_str);
  free(int_mtu_str);

  // IQ bitwidth
  char **match_list = NULL;
  size_t count = 0;
  get_ru_xml_list(buffer, "iq-bitwidth", &match_list, &count);
  const int16_t first_iq_width = (int16_t)atoi((char *)match_list[0]);
  free_match_list(match_list, count);

  // PRACH offset
  // xran_mplane->prach_offset

  // DU port ID bitmask
  xran_mplane->du_port_bitmask = 0xf000;
  // Band sector bitmask
  xran_mplane->band_sector_bitmask = 0x0f00;
  // CC ID bitmask
  xran_mplane->ccid_bitmask = 0x00f0;
  // RU port ID bitmask
  xran_mplane->ru_port_bitmask = 0x000f;

  // DU port ID
  xran_mplane->du_port = 0;
  // Band sector
  xran_mplane->band_sector = 0;
  // CC ID
  xran_mplane->ccid = 0;
  // RU port ID
  xran_mplane->ru_port = 0;

  // Frame structure
  match_list = NULL;
  count = 0;
  get_ru_xml_list(buffer, "supported-frame-structures", &match_list, &count);
  xran_mplane->frame_str = (int16_t)atoi((char *)match_list[0]);
  free_match_list(match_list, count);

  // Managed delay support
  char *managed_delay = get_ru_xml_node(buffer, "managed-delay-support");
  xran_mplane->managed_delay = (strcasecmp(managed_delay, "NON_MANAGED") == 0) ? false : true;
  free(managed_delay);

  // Store the max gain
  char *max_tx_gain_str = get_ru_xml_node(buffer, "max-gain");
  xran_mplane->max_tx_gain = (double)atof(max_tx_gain_str);
  free(max_tx_gain_str);

  // Model name
  char *model_name = get_ru_xml_node(buffer, "model-name");

  if (strcasecmp(ru_vendor, "BENETEL") == 0 /* || strcmp(ru_vendor, "VVDN-LPRU") == 0 || strcmp(ru_vendor, "Metanoia") == 0 */) {
    fix_benetel_setting(xran_mplane, interface_mtu, first_iq_width, max_num_ant, model_name);
  } else {
    AssertError(false, return false, "[MPLANE] %s RU currently not supported.\n", ru_vendor);
  }

  MP_LOG_I("Storing the following information to forward to xran:\n\
    RU MAC address %s\n\
    MTU %d\n\
    Compression header type %s\n\
    IQ bitwidth %d\n\
    PRACH offset %d\n\
    DU port bitmask %d\n\
    Band sector bitmask %d\n\
    CC ID bitmask %d\n\
    RU port ID bitmask %d\n\
    DU port ID %d\n\
    Band sector ID %d\n\
    CC ID %d\n\
    RU port ID %d\n\
    max Tx gain %.1f\n",
      xran_mplane->ru_mac_addr,
      xran_mplane->mtu,
      xran_mplane->comp_hdr_type == 0 ? "dynamic" : "static",
      xran_mplane->iq_width,
      xran_mplane->prach_offset,
      xran_mplane->du_port_bitmask,
      xran_mplane->band_sector_bitmask,
      xran_mplane->ccid_bitmask,
      xran_mplane->ru_port_bitmask,
      xran_mplane->du_port,
      xran_mplane->band_sector,
      xran_mplane->ccid,
      xran_mplane->ru_port,
      xran_mplane->max_tx_gain);

  free(ru_vendor);
  free(model_name);

  return true;
}

bool get_uplane_info(const char *buffer, ru_mplane_config_t *ru_mplane_config)
{
  // Interface name
  ru_mplane_config->interface_name = get_ru_xml_node(buffer, "interface");

  // PDSCH
  uplane_info_t *tx_end = &ru_mplane_config->tx_endpoints;
  get_ru_xml_list(buffer, "static-low-level-tx-endpoints", &tx_end->name, &tx_end->num);
  AssertError(tx_end->name != NULL, return false, "[MPLANE] Cannot get TX endpoint names.\n");

  // TX carriers
  uplane_info_t *tx_carriers = &ru_mplane_config->tx_carriers;
  get_ru_xml_list(buffer, "tx-arrays", &tx_carriers->name, &tx_carriers->num);
  AssertError(tx_carriers->name != NULL, return false, "[MPLANE] Cannot get TX carrier names.\n");

  // PUSCH and PRACH
  uplane_info_t *rx_end = &ru_mplane_config->rx_endpoints;
  get_ru_xml_list(buffer, "static-low-level-rx-endpoints", &rx_end->name, &rx_end->num);
  AssertError(rx_end->name != NULL, return false, "[MPLANE] Cannot get RX endpoint names.\n");

  // RX carriers
  uplane_info_t *rx_carriers = &ru_mplane_config->rx_carriers;
  get_ru_xml_list(buffer, "rx-arrays", &rx_carriers->name, &rx_carriers->num);
  AssertError(rx_carriers->name != NULL, return false, "[MPLANE] Cannot get RX carrier names.\n");

  MP_LOG_I("Successfully retrieved all the U-plane info - interface name, TX/RX carrier names, and TX/RX endpoint names.\n");

  return true;
}

bool get_pm_object_list(const char *buffer, pm_stats_t *pm_stats)
{
  char *ru_vendor = get_ru_xml_node(buffer, "mfg-name");
  if (strcasecmp(ru_vendor, "BENETEL") == 0) {
    pm_stats->start_up_timing = false;
  } else {
    pm_stats->start_up_timing = true;
  }

  // Rx window
  get_ru_xml_list(buffer, "rx-window-objects", &pm_stats->rx_window_meas, &pm_stats->rx_num);

  // Tx
  get_ru_xml_list(buffer, "tx-stats-objects", &pm_stats->tx_meas, &pm_stats->tx_num);

  MP_LOG_I("Successfully retreived all performance measurement names.\n");

  free(ru_vendor);

  return true;
}
