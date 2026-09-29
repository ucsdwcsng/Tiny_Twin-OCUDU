<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Overview

This library implements ZMQ-based radio driver

# Architecture

The radio simulates RX/TX pairs between different processes over network or on the same machine
as ZMQ REQ/REP socket pairs. All the antennas have to be configured and connected before the
simulation can start.

# Limitations

You cannot reconnect a device once the simulation has started. The simulation has to be restarted
from scratch in order to add devices or restart a process.

# Requirements

Depends on zmq library being installed in the system. On ubuntu: `libzmq3-dev`, on RHEL/Fedora: `zeromq-devel`

# Usage

## Simple 1-to-1 antenna mapping

Add `--device.name oai_zmqdevif` to load the library in UE / gNB process. Add `--zmq.[0].tx_channels <channels>` and
`--zmq.[0].rx_channels <channels>` to define ZMQ REQ/REP pairs.

On the opposite side, load the library like specified above but invert the `rx` and `tx` channels. This way all
antennas of UE will be directly mapped to all antennas of the gNB.

### Example:

```
sudo ./nr-uesoftmodem -r 106 --numerology 1 --band 78 -C 3619200000 --device.name oai_zmqdevif --zmq.[0].tx_channels tcp://127.0.0.1:4557 --zmq.[0].rx_channels tcp://127.0.0.1:4556 --ssb 516
```

```
sudo ./nr-softmodem -O ../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf --gNBs.[0].min_rxtxtime 6 --device.name oai_zmqdevif --zmq.[0].tx_channels tcp://127.0.0.1:4556 --zmq.[0].rx_channels tcp://127.0.0.1:4557
```

## Multi-tap channel

Channel-effect processing is enabled by default. Set
`--zmq.[0].channel_effects_enabled 0` on the command line, or add
`channel_effects_enabled = false;` to the corresponding `zmq` entry in the
configuration file, to bypass TX and RX taps, trace replay, path gain, and
noise. Set it to `1` or `true` to enable processing again. The option is read
when the radio device starts, so changing it requires restarting the UE.

`--zmq.[0].tx_taps '1,0;0.5,0'` sets TX taps to `1` and `0.5`, at
sample delays 0 and 1. `--zmq.[0].rx_taps '1,0;0,0.5'` sets RX taps to
`1` and `0.5j`. Each setting accepts 1–20 finite `real,imag` pairs separated
by semicolons; quote the argument in a shell. Omitted settings default to
`1,0` (identity). The same coefficients apply to every antenna in that
direction, with independent sample history per antenna.

The filter computes `y[n] = sum(h[k] * x[n-k])`, preserves history across
blocks, and saturates each output component to the signed 16-bit range.
TX filtering includes alignment silence so delayed tails remain at their
correct timestamps between bursts. Input TX buffers are not modified.

Configure a propagation effect once per link direction: for example, use
both settings on the UE and leave the gNB at identity. Configuring TX at
one endpoint and RX at the other cascades the two filters. Antenna streams
are filtered independently; there is no cross-antenna mixing. These settings provide fixed taps. Trace replay is described below.

## Tiny_Twin trace replay

Each direction can replay a whitespace-separated tap file instead of fixed taps:

```text
zmq = ({
  tx_channels = ("tcp://0.0.0.0:4557");
  rx_channels = ("tcp://10.53.1.3:4556");
  rx_tap_file = "/traces/channel_clean.txt";
  rx_tap_count = 10;
  rx_path_gain_db = 0.0;
  rx_noise_power_db = -10.0;
  rx_noise_seed = 1L;
  tx_tap_file = "/traces/channel_clean.txt";
  tx_tap_count = 10;
  tx_path_gain_db = 0.0;
  tx_noise_power_db = -10.0;
  tx_noise_seed = 2L;
});
```

Mount the trace directory into the UE container, for example
`/host/path/to/channel:/traces:ro`. Paths refer to files **inside the UE container**.
The image must contain a freshly built driver with trace support.

- `rx_tap_file` / `tx_tap_file`: real coefficient trace; empty by default (fixed taps).
- `rx_tap_file_imag` / `tx_tap_file_imag`: imaginary trace. When omitted, read the
  same file for both components, as in Tiny_Twin `f0fd378`'s UE initialization.
  Thus a file value `0.5` produces `0.5+0.5j`, not a real-only `0.5` tap.
  For real-only coefficients, supply a separate imaginary file of zero rows.
- `rx_tap_count` / `tx_tap_count`: coefficients consumed per row (1–20; default 1).
  Extra columns are ignored. Missing columns retain real `[1,0,...]` and imaginary
  `[0,0,...]` defaults. Invalid/nonfinite coefficients cause initialization to fail.
- One row is applied to a complete RX callback or accepted, nonempty TX write.
  A trace takes precedence over fixed taps in that direction. Antennas have separate
  replay cursors and history. RX and TX also advance independently.
- No interpolation or wall-clock scheduling is used. Block sizes and callback rates
  determine the sample-time duration of a row. TX alignment silence uses the current
  coefficients and advances history without consuming another row. Rejected writes
  do not consume rows. Reset rewinds the trace and clears history.
- At EOF each exhausted component returns to its default; after both files end the
  filter is identity. There is no implicit looping or holding the final row.

The filter reproduces Tiny_Twin's post-convolution formulas literally:

```text
y = round(10^(path_gain_db/20) * sum(h[k] * x[n-k])
          + 256 * 10^(noise_power_db/10) * N(0,1))
```

The Gaussian draw is independent for each real/imaginary output component. Despite
its name, `noise_power_db` determines the above amplitude multiplier, not an SNR
setting or the usual power-to-standard-deviation conversion. Path gain defaults to
0 dB; noise defaults to disabled (`-inf` internally). Set the two values to those
of the Tiny_Twin channel descriptor you are comparing: its `path_loss_dB` is used
as a gain, including its sign. Trace mode rounds to the nearest integer (half away
from zero), then preserves the driver's signed-16-bit saturation protection.

Compatibility scope: SISO tap replay and the gain/noise formulas in Tiny_Twin
`f0fd378`'s `rxAddInput`/`txAddInput`. Files are loaded and validated at initialization
and shared as immutable data to avoid filesystem access in the radio processing
threads; edits to a trace file require restart. Noise uses a per-channel seeded
C++ Gaussian generator, so its distribution/scaling matches but individual random
samples are not bit-identical to Tiny_Twin's global Ziggurat RNG. Saturation and
safe cross-block TX history are retained. Satellite/Doppler effects, a separate
propagation-delay offset, and cross-antenna mixing are not implemented here.
