/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "gtest/gtest.h"
extern "C" {
#include <stdlib.h>
#include "openair1/PHY/defs_nr_common.h"
#include "openair1/PHY/INIT/nr_parms.h"
#include "common/config/config_userapi.h"

static softmodem_params_t softmodem_params;
softmodem_params_t *get_softmodem_params(void)
{
  return &softmodem_params;
}
}

uint32_t ref_get_samples_slot_timestamp(NR_DL_FRAME_PARMS *fp, unsigned int slot)
{
  uint32_t samp_count = 0;
  for (unsigned int i = 0; i < slot; i++) {
    samp_count += get_samples_per_slot(i, fp);
  }
  return samp_count;
}

uint32_t ref_get_slot_from_timestamp(openair0_timestamp_t ts, NR_DL_FRAME_PARMS *fp)
{
  uint32_t slot_idx = 0;
  int samples_till_the_slot = get_samples_per_slot(slot_idx, fp) - 1;
  ts = ts % fp->samples_per_frame;

  while (ts > samples_till_the_slot) {
    slot_idx++;
    samples_till_the_slot += get_samples_per_slot(slot_idx, fp);
  }
  return slot_idx;
}

void test_coherence_symbol_api(NR_DL_FRAME_PARMS *fp)
{
  EXPECT_EQ(get_samples_symbol_timestamp(fp, 0, 0), get_samples_slot_timestamp(fp, 0));

  for (int slot = 0; slot < 4; slot++) {
    EXPECT_EQ(get_samples_symbol_duration(fp, slot, 0, fp->symbols_per_slot) + get_samples_slot_timestamp(fp, slot),
              get_samples_slot_timestamp(fp, slot + 1));
  }

  for (int symbol = 0; symbol < fp->symbols_per_slot - 1; symbol++) {
    EXPECT_EQ(get_samples_symbol_timestamp(fp, 0, symbol + 1) - get_samples_symbol_timestamp(fp, 0, symbol),
              get_samples_symbol_duration(fp, 0, symbol, 1));
  }
}

void test_coherence_slot_api(NR_DL_FRAME_PARMS *fp)
{
  for (unsigned int slot = 0; slot < fp->slots_per_frame; slot++) {
    EXPECT_EQ(get_samples_slot_timestamp(fp, slot), ref_get_samples_slot_timestamp(fp, slot));
  }

  openair0_timestamp_t ts = get_samples_per_slot(0, fp);
  // When timestamp is the last sample of the slot
  EXPECT_EQ(get_slot_from_timestamp(ts - 1, fp), 0);
  for (unsigned int slot = 0; slot < fp->slots_per_frame; slot++) {
    // When timestamp is the first sample of the slot
    ts = ref_get_samples_slot_timestamp(fp, slot);
    EXPECT_EQ(get_slot_from_timestamp(ts, fp), slot);
    // When timestamp is anywhere in the slot
    ts += (rand() % fp->samples_per_slotN0);
    EXPECT_EQ(get_slot_from_timestamp(ts, fp), slot);
  }
}

TEST(nr_frame_params, test_mu_3)
{
  NR_DL_FRAME_PARMS fp;
  int mu = NR_MU_3;
  memset(&fp, 0, sizeof(fp));
  nfapi_nr_config_request_scf_t cfg;
  cfg.carrier_config.num_rx_ant.value = 4;
  cfg.carrier_config.num_tx_ant.value = 4;
  cfg.carrier_config.dl_grid_size[mu].value = 66;
  cfg.carrier_config.ul_grid_size[mu].value = 66;
  cfg.cell_config.frame_duplex_type.value = TDD;
  cfg.ssb_config.scs_common.value = mu;
  fp.dl_CarrierFreq = 27524520000;
  nr_init_frame_parms(&cfg, &fp);
  nr_dump_frame_parms(&fp);
  test_coherence_symbol_api(&fp);
  test_coherence_slot_api(&fp);
}

TEST(nr_frame_params, test_mu_2)
{
  NR_DL_FRAME_PARMS fp;
  int mu = NR_MU_2;
  memset(&fp, 0, sizeof(fp));
  nfapi_nr_config_request_scf_t cfg;
  cfg.carrier_config.num_rx_ant.value = 4;
  cfg.carrier_config.num_tx_ant.value = 4;
  cfg.carrier_config.dl_grid_size[mu].value = 51;
  cfg.carrier_config.ul_grid_size[mu].value = 51;
  cfg.cell_config.frame_duplex_type.value = TDD;
  cfg.ssb_config.scs_common.value = mu;
  fp.dl_CarrierFreq = 27524520000;
  nr_init_frame_parms(&cfg, &fp);
  nr_dump_frame_parms(&fp);
  test_coherence_symbol_api(&fp);
  test_coherence_slot_api(&fp);
}

TEST(nr_frame_params, test_mu_1)
{
  NR_DL_FRAME_PARMS fp;
  int mu = NR_MU_1;
  memset(&fp, 0, sizeof(fp));
  nfapi_nr_config_request_scf_t cfg;
  cfg.carrier_config.num_rx_ant.value = 4;
  cfg.carrier_config.num_tx_ant.value = 4;
  cfg.carrier_config.dl_grid_size[mu].value = 106;
  cfg.carrier_config.ul_grid_size[mu].value = 106;
  cfg.cell_config.frame_duplex_type.value = TDD;
  cfg.ssb_config.scs_common.value = mu;
  fp.dl_CarrierFreq = 3600000000;
  nr_init_frame_parms(&cfg, &fp);
  nr_dump_frame_parms(&fp);
  test_coherence_symbol_api(&fp);
  test_coherence_slot_api(&fp);
}

TEST(nr_frame_params, test_mu_0)
{
  int mu = NR_MU_0;
  NR_DL_FRAME_PARMS fp;
  memset(&fp, 0, sizeof(fp));
  nfapi_nr_config_request_scf_t cfg;
  cfg.carrier_config.num_rx_ant.value = 4;
  cfg.carrier_config.num_tx_ant.value = 4;
  cfg.carrier_config.dl_grid_size[mu].value = 216;
  cfg.carrier_config.ul_grid_size[mu].value = 216;
  cfg.cell_config.frame_duplex_type.value = TDD;
  cfg.ssb_config.scs_common.value = mu;
  fp.dl_CarrierFreq = 2600000000;
  nr_init_frame_parms(&cfg, &fp);
  nr_dump_frame_parms(&fp);
  test_coherence_symbol_api(&fp);
  test_coherence_slot_api(&fp);
}

int main(int argc, char **argv)
{
  uniqCfg = load_configmodule(argc, argv, CONFIG_ENABLECMDLINEONLY);
  logInit();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
