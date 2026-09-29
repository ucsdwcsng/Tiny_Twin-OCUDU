/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
  RRC_config_tools.h
  -------------------
  AUTHOR  : Francois TABURET
  COMPANY : NOKIA BellLabs France
  EMAIL   : francois.taburet@nokia-bell-labs.com
*/
#ifndef RRC_CONFIG_TOOLS_H_
#define RRC_CONFIG_TOOLS_H_

#define KHz (1000UL)
#define MHz (1000*KHz)

typedef struct eutra_band_s {
  int16_t             band;
  uint32_t            ul_min;
  uint32_t            ul_max;
  uint32_t            dl_min;
  uint32_t            dl_max;
  frame_type_t        frame_type;
} eutra_band_t;


extern int config_check_band_frequencies(int ind, int16_t band, uint32_t downlink_frequency, 
                                         int32_t uplink_frequency_offset, uint32_t  frame_type);

extern int config_check_assign_DLGap_NB(paramdef_t *param);
extern int config_check_assign_rach_NB(paramdef_t *param);
#endif
