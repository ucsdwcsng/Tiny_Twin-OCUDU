/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief macro definitions for RRC authorized and asn1 parameters values, to be used in paramdef_t/chechedparam_t structure initializations 
 */
#ifndef __RRC_PARAMSVALUES__H__
#define __RRC_PARAMSVALUES__H__
/*    cell configuration section name */
#define ENB_CONFIG_STRING_ENB_LIST                      "eNBs"
/* component carriers configuration section name */		
#define ENB_CONFIG_STRING_COMPONENT_CARRIERS                            "component_carriers"		 


#define ENB_CONFIG_STRING_FRAME_TYPE                                    "frame_type"
#define ENB_CONFIG_STRING_PBCH_REPETITION                               "pbch_repetition"
#define ENB_CONFIG_STRING_TDD_CONFIG                                    "tdd_config"
#define ENB_CONFIG_STRING_TDD_CONFIG_S                                  "tdd_config_s"
#define ENB_CONFIG_STRING_PREFIX_TYPE                                   "prefix_type"
#define ENB_CONFIG_STRING_EUTRA_BAND                                    "eutra_band"
#define ENB_CONFIG_STRING_DOWNLINK_FREQUENCY                            "downlink_frequency"
#define ENB_CONFIG_STRING_UPLINK_FREQUENCY_OFFSET                       "uplink_frequency_offset"
#define ENB_CONFIG_STRING_NID_CELL                                      "Nid_cell"
#define ENB_CONFIG_STRING_N_RB_DL                                       "N_RB_DL"
#define ENB_CONFIG_STRING_CELL_MBSFN                                    "Nid_cell_mbsfn"

#define TDDCFG(A)                                               TDD_Config__subframeAssignment_ ## A
#define TDDCONFIG_OKRANGE                                       { TDDCFG(sa0), TDDCFG(sa6)}   

#define TDDCFGS(A)                                              TDD_Config__specialSubframePatterns_ ## A
#define TDDCONFIGS_OKRANGE                                      { TDDCFGS(ssp0), TDDCFGS(ssp8)}   

#define UETIMER_T300_OKVALUES                                   {100,200,300,400,600,1000,1500,2000}
#define UETT300(A)                                              LTE_UE_TimersAndConstants__t300_ ## A
#define UETIMER_T300_MODVALUES                                  { UETT300(ms100), UETT300(ms200),UETT300(ms300),UETT300(ms400),UETT300(ms600),UETT300(ms1000),UETT300(ms1500),UETT300(ms2000)}           		

#define UETIMER_T301_OKVALUES                                   {100,200,300,400,600,1000,1500,2000}
#define UETT301(A)                                              LTE_UE_TimersAndConstants__t301_ ## A
#define UETIMER_T301_MODVALUES                                  { UETT301(ms100), UETT301(ms200),UETT301(ms300),UETT301(ms400),UETT301(ms600),UETT301(ms1000),UETT301(ms1500),UETT301(ms2000)}

#define UETIMER_T310_OKVALUES                                   {0,50,100,200,500,1000,2000}
#define UETT310(A)                                              LTE_UE_TimersAndConstants__t310_ ## A
#define UETIMER_T310_MODVALUES                                  { UETT310(ms0), UETT310(ms50),UETT310(ms100),UETT310(ms200),UETT310(ms500),UETT310(ms1000),UETT310(ms2000)}

#define UETIMER_T311_OKVALUES                                   {1000,3000,5000,10000,15000,20000,30000}
#define UETT311(A)                                              LTE_UE_TimersAndConstants__t311_ ## A
#define UETIMER_T311_MODVALUES                                  { UETT311(ms1000), UETT311(ms3000), UETT311(ms5000), UETT311(ms10000), UETT311(ms15000), UETT311(ms20000), UETT311(ms30000)}

#define UETIMER_N310_OKVALUES                                   {1,2,3,4,6,8,10,20}
#define UETN310(A)                                              LTE_UE_TimersAndConstants__n310_ ## A
#define UETIMER_N310_MODVALUES                                  { UETN310(n1), UETN310(n2),UETN310(n3),UETN310(n4),UETN310(n6),UETN310(n8),UETN310(n10),UETN310(n20)}

#define UETIMER_N311_OKVALUES                                   {1,2,3,4,5,6,8,10}
#define UETN311(A)                                              LTE_UE_TimersAndConstants__n311_ ## A
#define UETIMER_N311_MODVALUES                                  { UETN311(n1), UETN311(n2),UETN311(n3),UETN311(n4),UETN311(n5),UETN311(n6),UETN311(n8),UETN311(n10)}

#endif
