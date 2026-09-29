<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# UpLink Multiple Input Multiple Output (UL MIMO)
## What is UL MIMO ?

UpLink MIMO refers to the use of multiple antennas at both the UE (User Equipment) as transmitter and the base station as receiver. They are used to send multiple data streams simultaneously from the user to the network.

## How to use it ?

## Step 1: build phy sim

```bash
cd cmake_targets/
sudo ./build_oai --phy_simulators -c
cd phy_simulators/build/
```

## Step 2: Use the sim

### Option 1: launch ULSCH sim  
On UpLink Shared CHannel(ULSCH), multiple users share the same radio resources (time and frequency slots), the base station schedules and assigns the resources dynamically. The goal here is to simulate the sending of data from an UE to a base station. 

Example:

| Parameter | Value | Description              |
|-----------|:-----:|--------------------------|
| R         |  106  | R_NB_UL = 106            |
| m         |   9   | MCS = 9                  |
| s         |  13   | Start SNR                |
| n         | 100   | Will simulate 100 frames |
| y         |   4   | 4 antennas used in eNB   |
| z         |   4   | 4 antennas used in UE    |
| W         |   4   | Will use 4 layers        |



```bash
sudo ./nr_ulschsim -R 106 -m9 -s13 -n100 -y4 -z4 -W4
```

### Option 2: launch UL sim
On UpLink Channel, a single user is dealing with a single base station. The goal is here to simulate the sending of the data from an UE to a base station.

It's focusing on: 

- UpLink chain validation  
- PUSCH (whole chain) on UE side and gNB side
- No channel model on data domain signal, which means an ideal scenario is used
- No cross-path connection, which means no signal leakage or interference are present
- PMI = 0  is only unitary precoding matrix

The same parameters are used.

```bash
sudo ./nr_ulsim -n100 -m9 -r106 -s13 -W4 -y4 -z4   
```
### Option 3: Use RFSIM

#### Build

RF simulator allows to test OAI without an RF board. It replaces an actual RF board driver.

```bash
sudo ./build_oai -c --gNB --nrUE -w SIMU
```

#### Run
##### 4x4 RANK 4

Example:

| Parameter     | Value | Description                                              |
|---------------|:-----:|----------------------------------------------------------|
| O             | [...] | Use a configuration file                                 |
| l             |   2   | Set the number of layers for downlink to 2 for PHY test mode |
| L             |   4   | Set the number of layers for uplink to 4 for PHY test mode   |
| ue-nb-ant-rx  |   4   | Set UE number of rx antennas to 4                         |
| ue-nb-ant-tx  |   4   | Set UE number of tx antennas to 4                         |


```bash
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.4layer.conf --rfsim --phy-test --l 2  --L 4
sudo ./nr-uesoftmodem  --rfsim --phy-test --ue-nb-ant-rx 4 --ue-nb-ant-tx 4
```

##### 4x4 RANK 2

The same parameters are used excepted uplink number of layers set to 2.

```bash
sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.4layer.conf --rfsim --phy-test --l 2  --L 2
sudo ./nr-uesoftmodem  --rfsim --phy-test --ue-nb-ant-rx 4 --ue-nb-ant-tx 4
```


