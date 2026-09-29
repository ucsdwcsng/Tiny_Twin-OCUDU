/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
* \brief Tools to query the PRACH configuration tables
* \note The PRACH configuration tables are used in the MAC layer for scheduling and beyond.
*       For example the PRACH duration is requested in the O-RAN 7.2 FrontHaul Interface
*/

#ifndef __NR_PRACH_CONFIG_H__
#define __NR_PRACH_CONFIG_H__

/**
 * @brief Fetch PRACH format (format only) from PRACH configuration tables
 *
 * @param index PRACH configuration index
 * @param unpaired Duplex mode TDD or FDD
 * @param freq_range Frequency range FR1 or FR2
 * @return PRACH format (format only)
 */
int get_format0(uint8_t index, uint8_t unpaired, frequency_range_t frequency_range);

typedef struct {
  uint32_t format;
  uint32_t start_symbol;
  uint32_t N_t_slot;
  uint32_t N_dur;
  uint32_t N_RA_slot;
  uint32_t N_RA_sfn;
  uint32_t max_association_period;
  int x;
  int y;
  int y2;
  uint64_t s_map;
} nr_prach_info_t;

/**
 * @brief Fetch PRACH occasion info from PRACH configuration tables
 *
 * @param index PRACH configuration index
 * @param freq_range Frequency range FR1 or FR2
 * @param unpaired Duplex mode TDD or FDD
 * @return PRACH occasion information
 */
nr_prach_info_t get_nr_prach_occasion_info_from_index(uint8_t index, frequency_range_t freq_range, uint8_t unpaired);

/**
 * @brief Fetch PRACH format (format concatenated with format2) from PRACH configuration tables
 *
 * @param index PRACH configuration index
 * @param pointa Point A ARFCN to determine the Frequency range FR1 or FR2
 * @param unpaired Duplex mode TDD or FDD
 * @return PRACH format (format concatenated with format2)
 */
uint16_t get_nr_prach_format_from_index(uint8_t index, uint32_t pointa, uint8_t unpaired);

#endif
