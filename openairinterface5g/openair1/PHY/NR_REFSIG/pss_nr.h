/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/**********************************************************************
*
* FILENAME    :  pss_nr.h
*
* MODULE      :  primary synchronisation signal
*
* DESCRIPTION :  elements related to pss
*
************************************************************************/

#ifndef PSS_NR_H
#define PSS_NR_H

#include "PHY/defs_nr_common.h"
#include "PHY/NR_REFSIG/ss_pbch_nr.h"
#include "PHY/defs_nr_UE.h"
/************** CODE GENERATION ***********************************/

//#define PSS_DECIMATOR                          /* decimation of sample is done between time correlation */

//#define CIC_DECIMATOR                          /* it allows enabling decimation based on CIC filter. By default, decimation is based on a FIF filter */

#define TEST_SYNCHRO_TIMING_PSS        (1)       /* enable time profiling */

//#define DBG_PSS_NR

/************** DEFINE ********************************************/

/* PROFILING */
#define TIME_PSS                      (0)
#define TIME_RATE_CHANGE              (TIME_PSS+1)
#define TIME_SSS                      (TIME_RATE_CHANGE+1)
#define TIME_LAST                     (TIME_SSS+1)

/* PSS configuration */

#define SYNCHRO_FFT_SIZE_MAX           (8192)                       /* maximum size of fft for synchronisation */

#define  NO_RATE_CHANGE                (1)

#ifdef PSS_DECIMATOR
  #define  RATE_CHANGE                 (SYNCHRO_FFT_SIZE_MAX/SYNCHRO_FFT_SIZE_PSS)
  #define  SYNCHRO_FFT_SIZE_PSS        (256)
  #define  OFDM_SYMBOL_SIZE_PSS        (SYNCHRO_FFT_SIZE_PSS)
  #define  SYNCHRO_RATE_CHANGE_FACTOR  (SYNCHRO_FFT_SIZE_MAX/SYNCHRO_FFT_SIZE_PSS)
  #define  CIC_FILTER_STAGE_NUMBER     (4)
#else
  #define  RATE_CHANGE                 (1)
  #define  SYNCHRO_RATE_CHANGE_FACTOR  (1)
#endif

typedef struct {
  c16_t **rxdata;
  int nb_antennas_rx;
  int rxdata_length;
  int ofdm_symbol_size;
  int nb_prefix_samples;
  int subcarrier_spacing;
  bool fo_flag;
  int target_Nid_cell;
  c16_t *pssTime;
} pss_search_t;

typedef struct {
  pss_detection_result_t pss_elem_info[NUMBER_PSS_SEQUENCE];
} nr_pss_info_t;

nr_pss_info_t pss_search_time_nr(const pss_search_t *p);

void generate_pss_nr_time(int ofdm_symbol_size,
                          int first_carrier_offset,
                          const int N_ID_2,
                          int ssbFirstSCS,
                          c16_t pssTime[ofdm_symbol_size]);
void generate_pss_nr(const int N_ID_2, int16_t *pss);
#endif /* PSS_NR_H */


