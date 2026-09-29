<!-- SPDX-License-Identifier: CC-BY-4.0 -->

The Duranta OpenAirInterface software can be obtained Github. You will
need a git client to get the sources. The repository is used for main
developments.

## Prerequisites

You need to install `git` using the following commands:

```shell
sudo apt-get update
sudo apt-get install git
```

## Clone the Git repository (for OAI Users without login to github server)

The [openairinterface5g repository](https://github.com/duranta-project/openairinterface5g.git)
holds the source code for the RAN (4G and 5G).

### All users, anonymous access

Clone the RAN repository:

```shell
git clone https://github.com/duranta-project/openairinterface5g.git
```

## Which branch to checkout?

- `develop`: contains recent commits that are tested on our CI test bench. The
  update frequency is about once a week. **This is the
  recommended and default branch.**

You can find the latest stable tag release [here](https://github.com/duranta-project/openairinterface5g/releases).

The tag naming conventions are:

- On `develop` branch **`YYYY.wXX`**
  * `YYYY` is the calendar year
  * `XX` the week number within the year
- On `develop` branch **`vX.Y`**
  * a semantic version number

More information on work flow and policies can be found in [this
document](./code-style-contrib.md).
