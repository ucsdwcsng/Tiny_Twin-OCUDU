/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! 
 * \brief Polar definitions required for Sidelink PSBCH
 */

#ifndef __NR_POLAR_PSBCH_DEFS__H__
#define __NR_POLAR_PSBCH_DEFS__H__

// PSBCH related polar parameters.

// PSBCH symbols sent in 11RBS, 9 symbols. 11*9*(12-3(for DMRS))*2bits = 1782 bits
#define SL_NR_POLAR_PSBCH_E_NORMAL_CP 1782
// PSBCH symbols sent in 11RBS, 7 symbols. 11*7*(12-3(for DMRS))*2bits = 1386 bits
#define SL_NR_POLAR_PSBCH_E_EXT_CP 1386
// SL_NR_POLAR_PSBCH_E_NORMAL_CP/32
#define SL_NR_POLAR_PSBCH_E_DWORD 56

#define SL_NR_POLAR_PSBCH_MESSAGE_TYPE (NR_POLAR_UCI_PUCCH_MESSAGE_TYPE + 1)
#define SL_NR_POLAR_PSBCH_PAYLOAD_BITS 32
#define SL_NR_POLAR_PSBCH_AGGREGATION_LEVEL 0
#define SL_NR_POLAR_PSBCH_N_MAX 9
#define SL_NR_POLAR_PSBCH_I_IL 1
#define SL_NR_POLAR_PSBCH_I_SEG 0
#define SL_NR_POLAR_PSBCH_N_PC 0
#define SL_NR_POLAR_PSBCH_N_PC_WM 0
#define SL_NR_POLAR_PSBCH_I_BIL 0
#define SL_NR_POLAR_PSBCH_CRC_PARITY_BITS 24
#define SL_NR_POLAR_PSBCH_CRC_ERROR_CORRECTION_BITS 3

#endif
