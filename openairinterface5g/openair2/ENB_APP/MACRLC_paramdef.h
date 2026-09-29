/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief definition of configuration parameters for all eNodeB modules
 */

#ifndef __ENB_APP_MACRLC_PARAMDEF__H__
#define __ENB_APP_MACRLC_PARAMDEF__H__

/*----------------------------------------------------------------------------------------------------------------------------------------------------*/


/* MACRLC configuration parameters names   */
#define CONFIG_STRING_MACRLC_CC                            "num_cc"
#define CONFIG_STRING_MACRLC_TRANSPORT_N_PREFERENCE        "tr_n_preference"
#define CONFIG_STRING_MACRLC_LOCAL_N_ADDRESS               "local_n_address"
#define CONFIG_STRING_MACRLC_REMOTE_N_ADDRESS              "remote_n_address"
#define CONFIG_STRING_MACRLC_LOCAL_N_PORTC                 "local_n_portc"
#define CONFIG_STRING_MACRLC_REMOTE_N_PORTC                "remote_n_portc"
#define CONFIG_STRING_MACRLC_LOCAL_N_PORTD                 "local_n_portd"
#define CONFIG_STRING_MACRLC_REMOTE_N_PORTD                "remote_n_portd"
#define CONFIG_STRING_MACRLC_TRANSPORT_S_PREFERENCE        "tr_s_preference"
#define CONFIG_STRING_MACRLC_LOCAL_S_ADDRESS               "local_s_address"
#define CONFIG_STRING_MACRLC_REMOTE_S_ADDRESS              "remote_s_address"
#define CONFIG_STRING_MACRLC_LOCAL_S_PORTC                 "local_s_portc"
#define CONFIG_STRING_MACRLC_REMOTE_S_PORTC                "remote_s_portc"
#define CONFIG_STRING_MACRLC_LOCAL_S_PORTD                 "local_s_portd"
#define CONFIG_STRING_MACRLC_REMOTE_S_PORTD                "remote_s_portd"
#define CONFIG_STRING_MACRLC_SCHED_MODE                    "scheduler_mode"
#define CONFIG_STRING_MACRLC_PUSCH10xSNR                   "puSch10xSnr"
#define CONFIG_STRING_MACRLC_PUCCH10xSNR                   "puCch10xSnr"
#define CONFIG_STRING_MACRLC_DEFAULT_SCHED_DL_ALGO         "default_sched_dl_algo"
#define CONFIG_STRING_MACRLC_UE_MULTIPLE_MAX               "ue_multiple_max"
#define CONFIG_STRING_MACRLC_USE_MCS_OFFSET                "use_mcs_offset"
#define CONFIG_STRING_MACRLC_BLER_TARGET_LOWER             "bler_target_lower"
#define CONFIG_STRING_MACRLC_BLER_TARGET_UPPER             "bler_target_upper"
#define CONFIG_STRING_MACRLC_MAX_UL_RB_INDEX               "max_ul_rb_index"
/*-------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                            MacRLC  configuration parameters                                                                           */
/*   optname                                            helpstr   paramflags    XXXptr              defXXXval                  type           numelt     */
/*-------------------------------------------------------------------------------------------------------------------------------------------------------*/
// clang-format off
#define MACRLCPARAMS_DESC { \
  {CONFIG_STRING_MACRLC_CC,                        NULL,               0,    .uptr=NULL,           .defintval=50011,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_TRANSPORT_N_PREFERENCE,    NULL,               0,    .strptr=NULL,         .defstrval="local_L1",       TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_LOCAL_N_ADDRESS,           NULL,               0,    .strptr=NULL,         .defstrval="127.0.0.1",      TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_REMOTE_N_ADDRESS,          NULL,               0,    .uptr=NULL,           .defstrval="127.0.0.2",      TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_LOCAL_N_PORTC,             NULL,               0,    .uptr=NULL,           .defintval=50010,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_REMOTE_N_PORTC,            NULL,               0,    .uptr=NULL,           .defintval=50010,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_LOCAL_N_PORTD,             NULL,               0,    .uptr=NULL,           .defintval=50011,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_REMOTE_N_PORTD,            NULL,               0,    .uptr=NULL,           .defintval=50011,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_TRANSPORT_S_PREFERENCE,    NULL,               0,    .strptr=NULL,         .defstrval="local_RRC",      TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_LOCAL_S_ADDRESS,           NULL,               0,    .uptr=NULL,           .defstrval="127.0.0.1",      TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_REMOTE_S_ADDRESS,          NULL,               0,    .uptr=NULL,           .defstrval="127.0.0.2",      TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_LOCAL_S_PORTC,             NULL,               0,    .uptr=NULL,           .defintval=50020,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_REMOTE_S_PORTC,            NULL,               0,    .uptr=NULL,           .defintval=50020,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_LOCAL_S_PORTD,             NULL,               0,    .uptr=NULL,           .defintval=50021,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_REMOTE_S_PORTD,            NULL,               0,    .uptr=NULL,           .defintval=50021,            TYPE_UINT,     0}, \
  {CONFIG_STRING_MACRLC_SCHED_MODE,                NULL,               0,    .strptr=NULL,         .defstrval="default",        TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_PUSCH10xSNR,               NULL,               0,    .iptr=NULL,           .defintval=200,              TYPE_INT,      0}, \
  {CONFIG_STRING_MACRLC_PUCCH10xSNR,               NULL,               0,    .iptr=NULL,           .defintval=200,              TYPE_INT,      0}, \
  {CONFIG_STRING_MACRLC_DEFAULT_SCHED_DL_ALGO,     NULL,               0,    .strptr=NULL,         .defstrval="round_robin_dl", TYPE_STRING,   0}, \
  {CONFIG_STRING_MACRLC_UE_MULTIPLE_MAX,           NULL,               0,    .iptr=NULL,           .defintval=4,                TYPE_INT,      0}, \
  {CONFIG_STRING_MACRLC_USE_MCS_OFFSET,            NULL,               0,    .iptr=NULL,           .defintval=1,                TYPE_INT,      0}, \
  {CONFIG_STRING_MACRLC_BLER_TARGET_LOWER,         NULL,               0,    .dblptr=NULL,         .defdblval=.5,               TYPE_DOUBLE,   0}, \
  {CONFIG_STRING_MACRLC_BLER_TARGET_UPPER,         NULL,               0,    .dblptr=NULL,         .defdblval=2,                TYPE_DOUBLE,   0}, \
  {CONFIG_STRING_MACRLC_MAX_UL_RB_INDEX,           NULL,               0,    .iptr=NULL,           .defintval=22,               TYPE_INT,      0}, \
}
// clang-format on

#define MACRLC_CC_IDX                                          0
#define MACRLC_TRANSPORT_N_PREFERENCE_IDX                      1
#define MACRLC_LOCAL_N_ADDRESS_IDX                             2
#define MACRLC_REMOTE_N_ADDRESS_IDX                            3
#define MACRLC_LOCAL_N_PORTC_IDX                               4
#define MACRLC_REMOTE_N_PORTC_IDX                              5
#define MACRLC_LOCAL_N_PORTD_IDX                               6
#define MACRLC_REMOTE_N_PORTD_IDX                              7
#define MACRLC_TRANSPORT_S_PREFERENCE_IDX                      8
#define MACRLC_LOCAL_S_ADDRESS_IDX                             9
#define MACRLC_REMOTE_S_ADDRESS_IDX                            10
#define MACRLC_LOCAL_S_PORTC_IDX                               11
#define MACRLC_REMOTE_S_PORTC_IDX                              12
#define MACRLC_LOCAL_S_PORTD_IDX                               13
#define MACRLC_REMOTE_S_PORTD_IDX                              14
#define MACRLC_SCHED_MODE_IDX                                  15
#define MACRLC_PUSCH10xSNR_IDX                                 16
#define MACRLC_PUCCH10xSNR_IDX                                 17
#define MACRLC_DEFAULT_SCHED_DL_ALGO_IDX                       18
#define MACRLC_UE_MULTIPLE_MAX_IDX                             19
#define MACRLC_USE_MCS_OFFSET_IDX                              20
#define MACRLC_BLER_TARGET_LOWER_IDX                           21
#define MACRLC_BLER_TARGET_UPPER_IDX                           22
#define MACRLC_MAX_UL_RB_INDEX_IDX                             23
/*---------------------------------------------------------------------------------------------------------------------------------------------------------*/

#endif
