<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Environment variables

OAI uses/supports a number of environment variables, documented in the following:

- `NFAPI_TRACE_LEVEL`: set the nfapi custom logging framework's log level; can be one of `error`, `warn`, `note`, `info`, `debug`. Default is `warn`.
- `NR_AWGN_RESULTS_DIR`: directory containing BLER curves for NR L2simulator channel modelling in SISO case
- `NR_MIMO2x2_AWGN_RESULTS_DIR`: directory containing BLER curves for NR L2simulator channel modelling in 2x2 MIMO case
- `OPENAIR_DIR`: should point to the root directory of OpenAirInterface; BLER curves for LTE L2sim channel emulation 
- `NVRAM_DIR`: directory to read/write NVRAM data in (5G) `nvram` tool; if not defined, will use `PWD` (working directory)
- `OAI_CONFIGMODULE`: can be used to pass the configuration file instead of `-O`
- `OAI_GDBSTACKS`: if defined when hitting an assertion (`DevAssert()`, `AssertFatal()`, ...), OAI will load `gdb` and provide a backtrace for every thread
- `OAI_RNGSEED`: provide a fixed seed for random number generators (RNG). If
  this variable is absent, a random seed will be read (from `/dev/urandom`).
- `USIM_DIR`: directory to read/write USIM data in (4G) `usim` tool; if not defined, will use `PWD` (working directory)

Furthermore, these variables appear in code that is not maintained and maybe not even compiled anywhere:

- `HOST`: alternative host to connect to, for CLI, if neither `REMADDR` nor `SSH_CLIENT` are defined
- `REMADDR`: host to connect to, for CLI client
- `SSH_CLIENT`: alternative host to connect to, for CLI, if `REMADDR` is not defined
- `USER`: user name in the command-line interface
- `rftestInputFile`: input file for the `calibration_test` tool
- `OPENAIR2_DIR` : OMV and OMG modules, obsolete
- `TITLE`: OTG module, obsolete
