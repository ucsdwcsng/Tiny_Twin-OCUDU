/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <libbladeRF.h>
#include "common_lib.h"
#include "log.h"

/*! \brief BladeRF specific data structure */
typedef struct {
  //! opaque BladeRF device struct. An empty ("") or NULL device identifier will result in the first encountered device being opened
  //! (using the first discovered backend)
  struct bladerf *dev;

  //! Number of buffers
  unsigned int num_buffers;
  //! Buffer size
  unsigned int buffer_size;
  //! Number of transfers
  unsigned int num_transfers;
  //! RX timeout
  unsigned int rx_timeout_ms;
  //! TX timeout
  unsigned int tx_timeout_ms;
  //! Metadata for RX
  struct bladerf_metadata meta_rx;
  //! Metadata for TX
  struct bladerf_metadata meta_tx;
  //! Sample rate
  unsigned int sample_rate;

  // --------------------------------
  // Debug and output control
  // --------------------------------
  //! Number of underflows
  int num_underflows;
  //! Number of overflows
  int num_overflows;
  //! number of RX errors
  int num_rx_errors;
  //! Number of TX errors
  int num_tx_errors;

  //! timestamp of current TX
  uint64_t tx_current_ts;
  //! timestamp of current RX
  uint64_t rx_current_ts;
  //! number of actual samples transmitted
  uint64_t tx_actual_nsamps;
  //! number of actual samples received
  uint64_t rx_actual_nsamps;
  //! number of TX samples
  uint64_t tx_nsamps;
  //! number of RX samples
  uint64_t rx_nsamps;
  //! number of TX count
  uint64_t tx_count;
  //! number of RX count
  uint64_t rx_count;
} brf_state_t;

/** @addtogroup _BLADERF_PHY_RF_INTERFACE_
 * @{
 */

#define BLADERF_CHECK(COND, FMT, ...)                                                        \
  if (COND) {                                                                                \
    LOG_I(HW, FMT "\n" __VA_OPT__(, ) __VA_ARGS__);                                          \
  } else {                                                                                   \
    LOG_E(HW, "Failed: " FMT ": %s\n" __VA_OPT__(, ) __VA_ARGS__, bladerf_strerror(status)); \
    return -1;                                                                               \
  }

const bladerf_format format = BLADERF_FORMAT_SC16_Q11_META;

static const char *ch_dir(bladerf_channel ch)
{
  switch (ch) {
    case BLADERF_CHANNEL_RX(0):
      return "RX0";
    case BLADERF_CHANNEL_RX(1):
      return "RX1";
    case BLADERF_CHANNEL_TX(0):
      return "TX0";
    case BLADERF_CHANNEL_TX(1):
      return "TX1";
    default:
      return "unknown";
  }
  return "unknown";
}

/*! \brief Start BladeRF
 * \param device the hardware to use
 * \returns 0 on success
 */
static int trx_brf_start(openair0_device_t *device)
{
  brf_state_t *brf = device->priv;
  int status;

  brf->meta_tx.flags = 0;

  /* Configure the device's TX module for use with the sync interface.
   * SC16 Q11 samples *with* metadata are used. */
  status = bladerf_sync_config(brf->dev, BLADERF_CHANNEL_TX(0), format,
                               brf->num_buffers, brf->buffer_size, brf->num_transfers, brf->tx_timeout_ms);
  BLADERF_CHECK(status == 0,
                "Set TX sync interface num_buffers %u, buffer_size %u, num_transfer %u, tx_timeout_ms %u",
                brf->num_buffers, brf->buffer_size, brf->num_transfers, brf->tx_timeout_ms);

  /* Configure the device's RX module for use with the sync interface.
   * SC16 Q11 samples *with* metadata are used. */
  status = bladerf_sync_config(brf->dev, BLADERF_CHANNEL_RX(0), format,
                               brf->num_buffers, brf->buffer_size, brf->num_transfers, brf->rx_timeout_ms);
  BLADERF_CHECK(status == 0,
                "Set RX sync interface num_buffers %u, buffer_size %u, num_transfer %u, rx_timeout_ms %u",
                brf->num_buffers, brf->buffer_size, brf->num_transfers, brf->rx_timeout_ms);

  /* We must always enable the TX module after calling bladerf_sync_config(), and
   * before  attempting to TX samples via  bladerf_sync_tx(). */
  status = bladerf_enable_module(brf->dev, BLADERF_CHANNEL_TX(0), true);
  BLADERF_CHECK(status == 0, "Enable TX0 module");

  /* We must always enable the RX module after calling bladerf_sync_config(), and
   * before  attempting to RX samples via  bladerf_sync_rx(). */
  status = bladerf_enable_module(brf->dev, BLADERF_CHANNEL_RX(0), true);
  BLADERF_CHECK(status == 0, "Enable RX module");

  // in case it was set to verbosity before, disable now because otherwise the
  // terminal will be flooded
  bladerf_log_set_verbosity(BLADERF_LOG_LEVEL_INFO);
  return 0;
}


/*! \brief Called to send samples to the BladeRF RF target
      \param device pointer to the device structure specific to the RF hardware target
      \param timestamp The timestamp at which the first sample MUST be sent
      \param buff Buffer which holds the samples
      \param nsamps number of samples to be sent
      \param cc index of the component carrier
      \param flags Ignored for the moment
      \returns 0 on success
*/
static int trx_brf_write(openair0_device_t *device, openair0_timestamp_t ptimestamp, void **buff, int nsamps, int cc, int flags)
{
  brf_state_t *brf = device->priv;

  /* BRF has only 1 rx/tx chaine : is it correct? TODO: now, handle also two! */
  int16_t *samples = (int16_t *)buff[0];
  ptimestamp -= device->openair0_cfg->command_line_sample_advance - device->openair0_cfg->tx_sample_advance;

  // When  BLADERF_META_FLAG_TX_NOW is used the timestamp is not used, so one can't schedule a tx
  if (brf->meta_tx.flags == 0)
    brf->meta_tx.flags = BLADERF_META_FLAG_TX_BURST_START;
  brf->meta_tx.timestamp = (uint64_t)(ptimestamp);

  int status = bladerf_sync_tx(brf->dev, samples, nsamps, &brf->meta_tx, 2 * brf->tx_timeout_ms);
  if (status == BLADERF_ERR_TIME_PAST) {
    brf->num_tx_errors++;
    LOG_W(HW, "Timestamp error: Requested timestamp %ld is in the past.\n", ptimestamp);
  } else if (status != 0) {
    brf->num_tx_errors++;
    LOG_W(HW, "Error during TX: %s\n", bladerf_strerror(status));
  }

  if (brf->meta_tx.flags == BLADERF_META_FLAG_TX_BURST_START)
    brf->meta_tx.flags = BLADERF_META_FLAG_TX_UPDATE_TIMESTAMP;

  brf->tx_current_ts = brf->meta_tx.timestamp;
  brf->tx_actual_nsamps += brf->meta_tx.actual_count;
  brf->tx_nsamps += nsamps;
  brf->tx_count++;

  return nsamps; // brf->meta_tx.actual_count;
}

/*! \brief Receive samples from hardware.
 * Read \ref nsamps samples from each channel to buffers. buff[0] is the array for
 * the first channel. *ptimestamp is the time at which the first sample
 * was received.
 * \param device the hardware to use
 * \param[out] ptimestamp the time at which the first sample was received.
 * \param[out] buff An array of pointers to buffers for received samples. The buffers must be large enough to hold the number of samples \ref nsamps.
 * \param nsamps Number of samples. One sample is 2 byte I + 2 byte Q => 4 byte.
 * \param cc  Index of component carrier
 * \returns number of samples read
*/
static int trx_brf_read(openair0_device_t *device, openair0_timestamp_t *ptimestamp, void **buff, int nsamps, int cc)
{
  brf_state_t *brf = device->priv;

  // BRF has only one rx/tx chain
  int16_t *samples = (int16_t *)buff[0];

  brf->meta_rx.actual_count = 0;
  brf->meta_rx.flags = BLADERF_META_FLAG_RX_NOW;
  int status = bladerf_sync_rx(brf->dev, samples, (unsigned int)nsamps, &brf->meta_rx, 2 * brf->rx_timeout_ms);

  if (status != 0) {
    LOG_E(HW, "RX failed: %s\n", bladerf_strerror(status));
    brf->num_rx_errors++;
  } else if (brf->meta_rx.status & BLADERF_META_STATUS_OVERRUN) {
    brf->num_overflows++;
    LOG_W(HW,
          "%d RX overrun is detected. timestamp %ld. Requested %u but got %u samples\n",
          brf->num_overflows,
          brf->meta_rx.timestamp,
          brf->meta_rx.actual_count,
          nsamps);
  }

  if (brf->meta_rx.actual_count != nsamps) {
    LOG_E(HW, "RX bad samples count, wanted %d, got %d\n", nsamps, brf->meta_rx.actual_count);
  }

  brf->rx_current_ts = brf->meta_rx.timestamp;
  brf->rx_actual_nsamps += brf->meta_rx.actual_count;
  brf->rx_nsamps += nsamps;
  brf->rx_count++;

  *ptimestamp = brf->meta_rx.timestamp;

  return nsamps; // brf->meta_rx.actual_count;
}

/*! \brief Terminate operation of the BladeRF transceiver -- free all associated resources
 * \param device the hardware to use
 */
static void trx_brf_end(openair0_device_t *device)
{
  brf_state_t *brf = device->priv;
  // Disable RX module, shutting down our underlying RX stream

  int status;
  if ((status = bladerf_enable_module(brf->dev, BLADERF_CHANNEL_RX(0), false)) != 0)
    LOG_E(HW, "Failed: Disable RX module: %s\n", bladerf_strerror(status));
  if ((status = bladerf_enable_module(brf->dev, BLADERF_CHANNEL_TX(0), false)) != 0)
    LOG_E(HW, "Failed: Disable TX module: %s\n", bladerf_strerror(status));
  bladerf_close(brf->dev);
}


/*! \brief print the BladeRF statistics
* \param device the hardware to use
* \returns  0 on success
*/
static int trx_brf_get_stats(openair0_device_t *device)
{
  return 0;
}

/*! \brief Reset the BladeRF statistics
 * \param device the hardware to use
 * \returns  0 on success
 */
static int trx_brf_reset_stats(openair0_device_t *device)
{
  return 0;
}

/*! \brief Stop BladeRF
 * \param card the hardware to use
 * \returns 0 in success
 */
static int trx_brf_stop(openair0_device_t *device)
{
  return 0;
}

/*! \brief Set frequencies (TX/RX)
 * \param device the hardware to use
 * \param openair0_cfg1 openair0 Config structure (ignored. It is there to comply with RF common API)
 * \returns 0 in success
 */
static int trx_brf_set_freq(openair0_device_t *device, openair0_config_t *openair0_cfg1)
{
  int status;
  brf_state_t *brf = device->priv;
  openair0_config_t *openair0_cfg = (openair0_config_t *)device->openair0_cfg;

  unsigned int tx_freq = (unsigned int)openair0_cfg->tx_freq[0];
  status = bladerf_set_frequency(brf->dev, BLADERF_CHANNEL_TX(0), tx_freq);
  BLADERF_CHECK(status == 0, "Set TX frequency %u", tx_freq);

  unsigned int rx_freq = (unsigned int)openair0_cfg->rx_freq[0];
  status = bladerf_set_frequency(brf->dev, BLADERF_CHANNEL_RX(0), rx_freq);
  BLADERF_CHECK(status == 0, "Set RX frequency %u", rx_freq);

  return 0;
}


/*! \brief Set Gains (TX/RX)
 * \param device the hardware to use
 * \param openair0_cfg openair0 Config structure
 * \returns 0 in success
 */
static int trx_brf_set_gains(openair0_device_t *device, openair0_config_t *openair0_cfg)
{
  return 0;
}

static int trx_brf_write_init(openair0_device_t *device)
{
  return 0;
}

typedef struct channel_config {
    bladerf_channel channel;
    unsigned int frequency;
    unsigned int sample_rate;
    unsigned int bandwidth;
    int gain;
} channel_config_t;


static int configure_channel(struct bladerf *dev, channel_config_t *c)
{
  int status;
  unsigned int actual_value = 0;
  const char *dir = ch_dir(c->channel);

  status = bladerf_set_frequency(dev, c->channel, c->frequency);
  BLADERF_CHECK(status == 0, "Set %s frequency %u", dir, c->frequency);

  status = bladerf_set_sample_rate(dev, c->channel, c->sample_rate, &actual_value);
  BLADERF_CHECK(status == 0, "Set %s sample rate %u (is %u)", dir, c->sample_rate, actual_value);

  status = bladerf_set_bandwidth(dev, c->channel, c->bandwidth, &actual_value);
  BLADERF_CHECK(status == 0, "Set %s bandwidth %u (is %u)", dir, c->bandwidth, actual_value);

  status = bladerf_set_gain(dev, c->channel, c->gain);
  BLADERF_CHECK(status == 0, "Set %s gain %u", dir, c->gain);

  bladerf_gain g;
  status = bladerf_get_gain(dev, c->channel, &g);
  BLADERF_CHECK(status == 0, "Get %s gain: %u", dir, g);

  return 0;
}

/*! \brief Initialize Openair BLADERF target. It returns 0 if OK
 * \param device the hardware to use
 * \param openair0_cfg RF frontend parameters set by application
 * \returns 0 on success
 */
int device_init(openair0_device_t *device, openair0_config_t *openair0_cfg)
{
  int status;
  brf_state_t brf = {0};

  switch ((long) openair0_cfg->sample_rate) {
    case 46080000:
      openair0_cfg->samples_per_packet = 8192;
      openair0_cfg->tx_sample_advance = 0;
      brf.num_buffers = 128;
      brf.num_transfers = 32;
      brf.rx_timeout_ms = 2000;
      break;

    case 30720000:
      openair0_cfg->samples_per_packet = 4096;
      openair0_cfg->tx_sample_advance = 0;
      brf.num_buffers = 128;
      brf.num_transfers = 32;
      brf.rx_timeout_ms = 2000;
      break;

    case 23040000:
      openair0_cfg->samples_per_packet = 2048;
      openair0_cfg->tx_sample_advance = 0;
      brf.num_buffers = 64;
      brf.num_transfers = 16;
      brf.rx_timeout_ms = 2000;
      break;

    case 15360000:
      openair0_cfg->samples_per_packet = 2048;
      openair0_cfg->tx_sample_advance = 0;

      brf.num_buffers = 64;
      brf.num_transfers = 16;
      brf.rx_timeout_ms = 2000;
      break;
    case 11520000:
      openair0_cfg->samples_per_packet = 2048;
      openair0_cfg->tx_sample_advance = 0;
      brf.num_buffers = 64;
      brf.num_transfers = 16;
      brf.rx_timeout_ms = 2000;
      break;

    case 7680000:
      openair0_cfg->samples_per_packet = 1024;
      openair0_cfg->tx_sample_advance = 0;
      brf.num_buffers = 32;
      brf.num_transfers = 16;
      brf.rx_timeout_ms = 2000;
      break;

    case 1920000:
      openair0_cfg->samples_per_packet = 256;
      openair0_cfg->tx_sample_advance = 50;
      brf.num_buffers = 128;
      brf.num_transfers = 16;
      brf.rx_timeout_ms = 2000;
      break;

    default:
      LOG_E(HW, "Error: unknown sampling rate %f\n", openair0_cfg->sample_rate);
      return -1;
      break;
  }

  brf.tx_timeout_ms = 2000;
  brf.buffer_size = (unsigned int)openair0_cfg->samples_per_packet;
  brf.sample_rate   = (unsigned int)openair0_cfg->sample_rate;

  float rxterm = brf.num_transfers * brf.buffer_size * 1000.0 / brf.rx_timeout_ms;
  float txterm = brf.num_transfers * brf.buffer_size * 1000.0 / brf.tx_timeout_ms;
  LOG_I(HW,
        "sample rate %d trnsfers %d timeout rx %d tx %d buffer size %d (rx %f, tx %f)\n",
        brf.sample_rate,
        brf.num_transfers,
        brf.rx_timeout_ms,
        brf.tx_timeout_ms,
        brf.buffer_size,
        rxterm,
        txterm);
  DevAssert(brf.sample_rate > txterm * 1.25); // see doc on async interface
  DevAssert(brf.sample_rate > rxterm * 1.25);

  memset(&brf.meta_rx, 0, sizeof(brf.meta_rx));
  memset(&brf.meta_tx, 0, sizeof(brf.meta_tx));

  // set this to BLADERF_LOG_LEVEL_VERBOSE for more detailed BladeRF info
  bladerf_log_set_verbosity(BLADERF_LOG_LEVEL_INFO);

  status = bladerf_open(&brf.dev, openair0_cfg->sdr_addrs);
  BLADERF_CHECK(status == 0, "Init BladeRF device '%s'", openair0_cfg->sdr_addrs);

  if (bladerf_device_speed(brf.dev) != BLADERF_DEVICE_SPEED_SUPER) {
    LOG_E(HW, "Device does not operate at max speed, change the USB port\n");
    return -1;
    }

  /** [Forcing Tuning Mode to FPGA] */
  const bladerf_tuning_mode tMode = BLADERF_TUNING_MODE_FPGA;
  status = bladerf_set_tuning_mode(brf.dev, tMode);
  BLADERF_CHECK(status == 0, "Set tuning mode FPGA");

  status = bladerf_set_gain_mode(brf.dev, BLADERF_CHANNEL_RX(0), BLADERF_GAIN_MGC);
  BLADERF_CHECK(status == 0, "Disable AGC");

  channel_config_t rx_chan = {
    .channel = BLADERF_CHANNEL_RX(0),
    .frequency = openair0_cfg->rx_freq[0],
    .sample_rate = openair0_cfg->sample_rate,
    .bandwidth = openair0_cfg->rx_bw,
    .gain = openair0_cfg->rx_gain[0],
  };
  configure_channel(brf.dev, &rx_chan);

  // This is a hack. The USRP library currently does max_gain-tx_gain (tx_gain
  // is the attenuation, so completely illogical), but let's keep what users
  // would expect by doing the same. Existing configs can then be reused.
  // Note also that the doc says that get_gain_range() should be called after
  // setting the frequency, but the gain should normally come from outside, so
  // be set by user and independent of the exact frequency.
  const struct bladerf_range *g = NULL;
  status = bladerf_get_gain_range(brf.dev, BLADERF_CHANNEL_TX(0), &g);
  BLADERF_CHECK(status == 0 && g != NULL, "Get TX0 gain range");
  channel_config_t tx_chan = {
    .channel = BLADERF_CHANNEL_TX(0),
    .frequency = openair0_cfg->tx_freq[0],
    .sample_rate = openair0_cfg->sample_rate,
    .bandwidth = openair0_cfg->tx_bw,
    .gain = g->max - openair0_cfg->tx_gain[0], // see comment above
  };
  configure_channel(brf.dev, &tx_chan);

  LOG_I(HW, "BLADERF: Initializing device done\n");
  device->type             = BLADERF_DEV;
  device->trx_start_func = trx_brf_start;
  device->trx_end_func   = trx_brf_end;
  device->trx_read_func  = trx_brf_read;
  device->trx_write_func = trx_brf_write;
  device->trx_get_stats_func   = trx_brf_get_stats;
  device->trx_reset_stats_func = trx_brf_reset_stats;
  device->trx_stop_func        = trx_brf_stop;
  device->trx_set_freq_func    = trx_brf_set_freq;
  device->trx_set_gains_func   = trx_brf_set_gains;
  device->trx_write_init       = trx_brf_write_init;
  device->openair0_cfg = openair0_cfg;
  device->priv = malloc_or_fail(sizeof(brf_state_t));
  *(brf_state_t *)device->priv = brf;

  return 0;
}
/*@}*/
