# SPDX-License-Identifier: MIT

set -e
sudo cpupower idle-set -D 0 > /dev/null
sudo sysctl kernel.sched_rt_runtime_us=-1
sudo sysctl kernel.timer_migration=0
