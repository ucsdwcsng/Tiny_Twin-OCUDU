/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Top-level routines for generating and decoding the PUCCH physical channel V8.6 2009-03
 */

#ifndef __PHY_LTE_TRANSPORT_PUCCH_EXTERN__H__
#define __PHY_LTE_TRANSPORT_PUCCH_EXTERN__H__

#include <stdint.h>

/* PUCCH format3 >> */
#define D_NSLT1SF       2
#define D_NSYM1SLT      7
#define D_NSYM1SF       2*7
#define D_NSC1RB        12
#define D_NPUCCH_SF5    5
#define D_NPUCCH_SF4    4

extern const int16_t W4[3][4];

extern const int16_t W3_re[3][6];

extern const int16_t W3_im[3][6];

extern const int16_t alpha_re[12];
extern const int16_t alpha_im[12];

static char const* const pucch_format_string[] = {"format 1",
                                                  "format 1a",
                                                  "format 1b",
                                                  "pucch_format1b_csA2",
                                                  "pucch_format1b_csA3",
                                                  "pucch_format1b_csA4",
                                                  "format 2",
                                                  "format 2a",
                                                  "format 2b",
                                                  "pucch_format3"};

extern const uint8_t chcod_tbl[128][48];

extern const int16_t W5_fmt3_re[5][5];

extern const int16_t W5_fmt3_im[5][5];

extern const int16_t W4_fmt3[4][4];

extern const int16_t W2[2];

extern const int16_t RotTBL_re[4];
extern const int16_t RotTBL_im[4];

//np4_tbl, np5_tbl
extern const uint8_t Np5_TBL[5];
extern const uint8_t Np4_TBL[4];

// alpha_TBL
extern const int16_t alphaTBL_re[12];
extern const int16_t alphaTBL_im[12];

#endif
