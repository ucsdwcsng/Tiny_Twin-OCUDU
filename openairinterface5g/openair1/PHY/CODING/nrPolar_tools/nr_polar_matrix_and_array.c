/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "PHY/CODING/nrPolar_tools/nr_polar_defs.h"

void nr_matrix_multiplication_uint8_1D_uint8_2D(const uint8_t *matrix1,
                                                const uint8_t **matrix2,
                                                uint8_t *output,
                                                uint16_t row,
                                                uint16_t col)
{
  for (uint i = 0; i < col; i++) {
    output[i] = 0;
    for (uint j = 0; j < row; j++) {
      output[i] += matrix1[j] * matrix2[j][i];
    }
  }
}

// Modified Bubble Sort.
void nr_sort_asc_double_1D_array_ind(double *matrix, uint8_t *ind, uint8_t len)
{
  for (int i = 0; i < len; i++) {
    int swaps = 0;
    for (int j = 0; j < (len - i) - 1; j++) {
      if (matrix[j] > matrix[j + 1]) {
        double temp = matrix[j];
        matrix[j] = matrix[j + 1];
        matrix[j + 1] = temp;

        int tempInd = ind[j];
        ind[j] = ind[j + 1];
        ind[j + 1] = tempInd;

        swaps++;
      }
    }
    if (swaps == 0)
      break;
  }
}
