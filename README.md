# Tiny Twin OCUDU

**A software-only 5G SA testbed with trace-driven channel emulation and many UEs.**

Tiny Twin OCUDU is the [OCUDU](https://gitlab.com/ocudu/ocudu) version of [Tiny Twin](https://github.com/ucsdwcsng/Tiny_Twin). It runs a real 5G network entirely in software, with no radio hardware. An OCUDU gNB and [OpenAirInterface](https://openairinterface.org/) UEs exchange baseband IQ samples over ZeroMQ, with [Open5GS](https://open5gs.org/) as the core. Each UE passes its samples through a multi-tap channel emulator that replays measured or synthetic channel traces. You get the behaviour of a real protocol stack under realistic, repeatable radio conditions, on a single Linux machine using Docker Compose.

- **Channel emulation on the IQ samples.** Each UE applies a complex FIR of up to 20 taps to its downlink and uplink, replays a time-varying tap trace one row per slot, and adds path gain and noise. Every UE can have its own downlink and uplink trace.
- **Many UEs on one gNB.** A ZMQ broker copies the downlink to every UE and adds their uplinks together. Tested with 16 UEs.
- **Programmable scheduling.** An optional [jBPF](https://github.com/microsoft/jbpf) build of the gNB exposes downlink scheduler hooks. [EdgeRIC](https://edgeric.github.io/) apps running on [jrt-controller](https://github.com/microsoft/jrt-controller) use them to set per-UE PRB caps and MCS every slot.
- **Built-in measurement.** Includes per-slot gNB timing, a broker profiler, and scripts that sweep the number of UEs and plot the results.

Developed by [WCSNG](https://wcsng.ucsd.edu/) at UC San Diego. To get started, see [docs/quick-start.md](docs/quick-start.md).
