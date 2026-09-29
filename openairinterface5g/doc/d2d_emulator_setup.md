<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# How to use device-to-device communication (D2D, 4G)

---
>**⚠️ ATTENTION ⚠️**
> 
>D2D is currently unfinished, the following documentation steps are likely not enough to make it work.
---

## Scenario 1 : **Off-network UE2UE link**
SynchREF UE (UE1)

```mermaid
graph LR
    UE1[UE1<br/>eth0: 10.10.10.1] --- UE2[UE2<br/>eno1: 10.10.10.2]
```

### Example of /etc/network/interfaces configuration for UE1
```text
auto eth0 
   iface eth0 inet static 
   address 10.10.10.1 
   netmask 255.255.255.0 
   gateway 10.10.10.1 
```

### Prepare the environment:
```bash
git clone https://gitlab.eurecom.fr/matzakos/LTE-D2D.git
cd LTE-D2D
git checkout master
```  

This branch contains all the current development for **DDPS**, including:
 - **UE MAC <-> UE MAC** for **Scenario 1** (off-network communication)
 - **eNB MAC <-> UE MAC** using **NFAPI Transport** (on-network communication)
 - **RRC Extensions** to support on-network cases
 
### NFAPI configuration (required even for Scenario 1 target)
```bash
git clone https://github.com/cisco/open-nFAPI.git
cd open-nfapi
patch -p1 --dry-run < $OPENAIR_HOME/open-nfapi.oai.patch
```
Validate that there are no errors: 

```patch -p1 < $OPENAIR_HOME/open-nfapi.oai.patch```
 
### OAI build/execute
```bash
export NFAPI_DIR=XXX (place where NFAPI was installed)
cd cmake_targets
./build_oai --UE --ninja #If necessary, use ./build_oai -I --UE to install required packages
cd ran_build/build/
```

### UE1
``` bash
sudo ifconfig oip0 10.0.0.1
sudo iptables -A POSTROUTING  -t mangle -o oip0 -d 224.0.0.3 -j MARK --set-mark 3
```
---
>**NOTE**
>
>If necessary, add a default gateway: 
>
>```sudo route add default gw 10.10.10.1 eth0```
---


### UE2
```bash
sudo ifconfig oip0 10.0.0.2
sudo iptables -A POSTROUTING  -t mangle -o oip0 -d 224.0.0.3 -j MARK --set-mark 3
```
---
>**NOTE**
>
>If necessary, add a default gateway: 
>
>```sudo route add default gw 10.10.10.1 eno1```
---


### UE1 and UE2: Get and build `vencore_app` from `d2d-l3-stub` (branch: `l3_stub`)
```bash
gcc -I . vencore_app.c -o vencore_app -lpthread
```
 

## TEST ONE-TO-MANY

### Run UE1 then UE2, for example:
- UE1: 
```bash
sudo ./lte-softmodem-stub -U --emul-iface eth0
```
- UE2: 
```bash
sudo ./lte-softmodem-stub -U --emul-iface eno1
```

### Test with Ping
- Sender - UE1: 
```bash
ping -I oip0 224.0.0.3
```
- Receiver - UE2: *using wireshark*

### Test with Iperf
- Sender - UE1: 
```bash
iperf -c 224.0.0.3 -u -b 0.1M --bind 10.0.0.1 -t 100
```

- Receiver - UE2: 
```bash
sudo ./mcreceive 224.0.0.3 5001
``` 

Filter the incoming packets according to GroupL2Id: receiver (one-to-many) can discard the packets if it doesn't belong to this group. 
For the moment, both sender and receiver use the same set of Ids (hardcoded)  

- UE1 (sender):
  ```bash
  sudo ./lte-softmodem-stub -U --emul-iface eth0
  ./vencore_app  #send the sourceL2Id, groupL2Id to OAI
  ping -I oip0 224.0.0.3
  ```

- UE2(receiver)
  ```bash
  sudo ./lte-softmodem-stub -U --emul-iface eno1 
  #we can see the incomming packets from OAI log, however, cannot see from Wireshark -> they are discarded at MAC layer
  ./vencore_app  #we can see the packets appearing in Wireshark
  ```


## TEST PC5-S (UE1 -sender, UE2 - receiver) and PC5-U for ONE-TO-ONE scenario
### Configure UE1/UE2

Configure ports and routing table for UE1 and UE2

**UE1:** 
  ```bash
  sudo ifconfig oip0 10.0.0.1
  sudo iptables -A POSTROUTING  -t mangle -o oip0 -d 10.0.0.2 -j MARK --set-mark 3
  sudo route add default gw 10.10.10.1 eth0
  ```
**UE2:**
  ```bash
  sudo ifconfig oip0 10.0.0.2
  sudo iptables -A POSTROUTING  -t mangle -o oip0 -d 10.0.0.1 -j MARK --set-mark 3
  sudo route add default gw 10.10.10.1 eno1
  ```
  
#### Step 1:
Run the traffic emulated over Ethernet on UE1 using lte-softmodem-stub to test the behaviour without RF board.

**UE1:**
```bash
sudo ./lte-softmodem-stub -U --emul-iface eth0
```

#### Step 2:
Run the traffic emulated over Ethernet on UE2 and set it as listening to incoming messages from PC5-S

**UE2:** 
```bash
sudo ./lte-softmodem-stub -U --emul-iface eno1
./vencore_app -r #listen to incomming message from PC5-S
``` 

#### Step 3: 
Send a message from UE1 to UE2

**UE1:** 
```bash
./vencore_app -s #send a message via PC5-S (e.g., DirectCommunicationRequest)
```

#### Generate unicast traffic
**UE1:**
```bash
ping -I oip0 10.0.0.2
```
 
 
## TEST PC5-D

#### Step 1:
Run the traffic emulated over Ethernet on UE1 and send a discovery message via PC5-D 

**UE1:**
```bash
sudo ./lte-softmodem-stub -U --emul-iface eth0
./vencore_app -d #send a PC5-Discovery-Announcement via PC5D
```

#### Step 2:
Run the traffic emulated over Ethernet on UE2 and send a discovery message via PC5-D

**UE2:** 
```bash
sudo ./lte-softmodem-stub -U --emul-iface eno1
./vencore_app -d #send a PC5-Discovery-Announcement via PC5D
```

