/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "openair2/LAYER2/NR_MAC_COMMON/nr_mac_common.h"
#include "executables/softmodem-common.h"

/* Forward LUTs: const (external linkage) in nr_mac_common.c, not in the header. */
extern const int8_t lut_t1_r1[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE1_DMRS_MASK];
extern const int8_t lut_t1_r2[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE1_DMRS_MASK];
extern const int8_t lut_t1_r3[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE1_DMRS_MASK];
extern const int8_t lut_t1_r4[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE1_DMRS_MASK];
extern const int8_t lut_t2_r1[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE2_DMRS_MASK];
extern const int8_t lut_t2_r2[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE2_DMRS_MASK];
extern const int8_t lut_t2_r3[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE2_DMRS_MASK];
extern const int8_t lut_t2_r4[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE2_DMRS_MASK];
extern const int8_t lut_tp[MAX_FRONTLOAD_SYMB][MAX_CDM_GROUPS][MAX_TYPE1_DMRS_MASK];

softmodem_params_t *get_softmodem_params(void)
{
  return NULL;
}

/* non-NULL is dmrs type 2 to the decoder */
static long g_type2_marker = 1;

#define TP_DIS NR_PUSCH_Config__transformPrecoder_disabled
#define TP_EN NR_PUSCH_Config__transformPrecoder_enabled

typedef struct {
  const int8_t *lut;
  int mask_dim;
  uint8_t rank;
  /* gNB encode side: pusch_dmrs_type1 / type2 */
  int type;
  /* decode side: NULL = type1, non-NULL = type2 */
  const long *ue_type;
  int tp;
  const char *name;
} lut_desc_t;

/* Round-trip every live cell of one table: gNB encode reproduces val,
 * decode reproduces (cdm, mask, fl). abort() on any mismatch. */
static void check_lut(const lut_desc_t *t)
{
  int tested = 0;
  for (int f = 0; f < MAX_FRONTLOAD_SYMB; f++) {
    for (int c = 0; c < MAX_CDM_GROUPS; c++) {
      for (int m = 0; m < t->mask_dim; m++) {
        int8_t stored = t->lut[(f * MAX_CDM_GROUPS + c) * t->mask_dim + m];
        /* empty cell */
        if (stored == 0)
          continue;
        int val = stored - 1;
        tested++;

        int enc = get_dci_antenna_ports_val(t->rank, m, c + 1, t->type, f + 1, t->tp);
        if (enc != val) {
          fprintf(stderr, "FAIL %s encode: fl%d cdm%d mask%d -> %d, want %d\n", t->name, f + 1, c + 1, m, enc, val);
          abort();
        }

        uint8_t dcdm = 0;
        uint16_t dmask = 0;
        int dfl = 0;
        int ret = decode_dci_antenna_ports_val(t->rank, t->ue_type, (long)t->tp, (uint8_t)val, &dcdm, &dmask, &dfl);
        if (ret != 0 || dcdm != c + 1 || dmask != m || dfl != f + 1) {
          fprintf(stderr,
                  "FAIL %s decode val%d: ret%d cdm%d mask%d fl%d, want cdm%d mask%d fl%d\n",
                  t->name,
                  val,
                  ret,
                  dcdm,
                  dmask,
                  dfl,
                  c + 1,
                  m,
                  f + 1);
          abort();
        }
      }
    }
  }
  if (tested == 0) {
    fprintf(stderr, "FAIL %s: no live entries\n", t->name);
    abort();
  }
}

static void check_invalid(void)
{
  if (get_dci_antenna_ports_val(0, 1, 1, pusch_dmrs_type1, 1, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(5, 1, 1, pusch_dmrs_type1, 1, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(1, 1, 0, pusch_dmrs_type1, 1, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(1, 1, 4, pusch_dmrs_type1, 1, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(1, 1, 1, pusch_dmrs_type1, 0, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(1, 1, 1, pusch_dmrs_type1, 3, TP_DIS) != -1)
    abort();
  if (get_dci_antenna_ports_val(1, 0x7, 1, pusch_dmrs_type1, 1, TP_DIS) != -1)
    abort();

  uint8_t cdm;
  uint16_t mask;
  int fl;
  if (decode_dci_antenna_ports_val(1, NULL, TP_DIS, 14, &cdm, &mask, &fl) != -1)
    abort();
  if (decode_dci_antenna_ports_val(3, NULL, TP_DIS, 3, &cdm, &mask, &fl) != -1)
    abort();
}

static void lut_test(void)
{
  const lut_desc_t tables[] = {
      {(const int8_t *)lut_t1_r1, MAX_TYPE1_DMRS_MASK, 1, pusch_dmrs_type1, NULL, TP_DIS, "t1_r1"},
      {(const int8_t *)lut_t1_r2, MAX_TYPE1_DMRS_MASK, 2, pusch_dmrs_type1, NULL, TP_DIS, "t1_r2"},
      {(const int8_t *)lut_t1_r3, MAX_TYPE1_DMRS_MASK, 3, pusch_dmrs_type1, NULL, TP_DIS, "t1_r3"},
      {(const int8_t *)lut_t1_r4, MAX_TYPE1_DMRS_MASK, 4, pusch_dmrs_type1, NULL, TP_DIS, "t1_r4"},
      {(const int8_t *)lut_t2_r1, MAX_TYPE2_DMRS_MASK, 1, pusch_dmrs_type2, &g_type2_marker, TP_DIS, "t2_r1"},
      {(const int8_t *)lut_t2_r2, MAX_TYPE2_DMRS_MASK, 2, pusch_dmrs_type2, &g_type2_marker, TP_DIS, "t2_r2"},
      {(const int8_t *)lut_t2_r3, MAX_TYPE2_DMRS_MASK, 3, pusch_dmrs_type2, &g_type2_marker, TP_DIS, "t2_r3"},
      {(const int8_t *)lut_t2_r4, MAX_TYPE2_DMRS_MASK, 4, pusch_dmrs_type2, &g_type2_marker, TP_DIS, "t2_r4"},
      {(const int8_t *)lut_tp, MAX_TYPE1_DMRS_MASK, 1, pusch_dmrs_type1, NULL, TP_EN, "tp"},
  };

  for (unsigned i = 0; i < sizeof(tables) / sizeof(tables[0]); i++)
    check_lut(&tables[i]);

  check_invalid();
}

int main(void)
{
  lut_test();
  printf("All LUT round-trip tests passed.\n");
  return 0;
}
