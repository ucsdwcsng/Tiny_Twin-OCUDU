# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(polar STATIC
  nr_polar_init.c
  nr_bitwise_operations.c
  nr_crc_byte.c
  nr_polar_decoder.c
  nr_polar_decoding_tools.c
  nr_polar_encoder.c
  nr_polar_interleaving_pattern.c
  nr_polar_kernal_operation.c
  nr_polar_kronecker_power_matrices.c
  nr_polar_matrix_and_array.c
  nr_polar_procedures.c
  nr_polar_sequence_pattern.c
)
target_link_libraries(polar PRIVATE utils)
