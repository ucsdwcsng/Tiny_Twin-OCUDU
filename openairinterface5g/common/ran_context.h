/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief LTE Physical channel configuration and variable structure definitions
 */

#ifndef __RAN_CONTEXT_H__
#define __RAN_CONTEXT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <pthread.h>
#include <stdint.h>

// forward declarations to avoid including the full typess ***eNB;
struct PHY_VARS_gNB_s;
struct PHY_VARS_eNB_NB_IoT_s;
struct eNB_RRC_INST_s;
struct gNB_RRC_INST_s;
struct eNB_MAC_INST_s;
struct eNB_MAC_INST_NB_IoT_s;
struct gNB_MAC_INST_s;
struct RU_t_s;

typedef struct {
  /// RAN context config file name
  char *config_file_name;
  /// Number of RRC instances in this node
  int nb_inst;
  /// Number of NB_IoT RRC instances in this node
  int nb_nb_iot_rrc_inst;
  /// Number of NR RRC instances in this node
  int nb_nr_inst;
  /// Number of Component Carriers per instance in this node
  int *nb_CC;
  /// Number of NR Component Carriers per instance in this node
  int *nb_nr_CC;
  /// Number of MACRLC instances in this node
  int nb_macrlc_inst;
  /// Number of NB_IoT MACRLC instances in this node
  int nb_nb_iot_macrlc_inst;
  /// Number of NR MACRLC instances in this node
  int nb_nr_macrlc_inst;
  /// Number of component carriers per instance in this node
  int *nb_mac_CC;
  /// Number of L1 instances in this node
  int nb_L1_inst;
  /// Number of NB_IoT L1 instances in this node
  int nb_nb_iot_L1_inst;
  /// Number of NR L1 instances in this node
  int nb_nr_L1_inst;  
  /// Number of Component Carriers per instance in this node
  int *nb_L1_CC;
  /// Number of RU instances in this node
  int nb_RU;
  /// eNB context variables
  struct PHY_VARS_eNB_s ***eNB;
  /// gNB context variables
  struct PHY_VARS_gNB_s **gNB;
  /// NB_IoT L1 context variables
  struct PHY_VARS_eNB_NB_IoT_s **L1_NB_IoT;
  /// RRC context variables
  struct eNB_RRC_INST_s **rrc;
  /// NB_IoT RRC context variables
  //struct eNB_RRC_INST_NB_IoT_s **nb_iot_rrc;
  /// NR RRC context variables
  struct gNB_RRC_INST_s **nrrrc;
  /// MAC context variables
  struct eNB_MAC_INST_s **mac;
  /// NB_IoT MAC context variables
  struct eNB_MAC_INST_NB_IoT_s **nb_iot_mac;
  /// NR MAC context variables
  struct gNB_MAC_INST_s **nrmac;
  /// RU descriptors. These describe what each radio unit is supposed to do and contain the necessary functions for fronthaul interfaces
  struct RU_t_s **ru;
  /// Mask to indicate fronthaul setup status of RU (hard-limit to 64 RUs)
  uint64_t ru_mask;
  /// Mutex for protecting ru_mask
  pthread_mutex_t ru_mutex;
  /// condition variable for signaling setup completion of an RU
  pthread_cond_t ru_cond;
} RAN_CONTEXT_t;

extern RAN_CONTEXT_t RC;
#define NB_eNB_INST RC.nb_inst
#ifdef __cplusplus
}
#endif
#endif
