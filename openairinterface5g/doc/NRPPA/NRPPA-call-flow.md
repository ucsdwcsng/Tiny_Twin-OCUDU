<!-- SPDX-License-Identifier: CC-BY-4.0 -->

The sequence diagram below illustrates the NRPPA Call Flow for Uplink Time Difference of Arrival (UL-TDOA) based positioning implemented in duranta.

Note that the current implementation is restricted to a single gNB with multiple antennas as a distributed antenna system, where each antenna acts as a Transmission and reception point (TRP).

```mermaid
sequenceDiagram
    participant UE as UE
    participant TRP1 as TRP 1 <br/>(Serving Antenna/RU)
    participant TRP2 as TRP 2 <br/>(Neighbor Antenna/RU)
    participant DU as gNB-DU <br/>(Single Baseband)
    participant CU as gNB-CU
    participant AMF as AMF
    participant LMF as LMF
    participant API as External API

    API->>LMF:HTTP Post: <br> Determine Location (InputData)

    Note over UE, LMF:TRP Information Exchange
    LMF->>AMF: NRPPa TRP INFORMATION REQUEST
    AMF->>CU: NGAP Downlink Non-UE Associated NRPPa <br> (NRPPa TRP INFORMATION REQUEST)
    
    CU->>DU: F1AP TRP INFORMATION REQUEST <br> (Query TRP 1 & TRP 2)
    DU->>CU: F1AP TRP INFORMATION RESPONSE <br> (Lat/Long for TRP 1 & TRP 2)

    CU->>AMF: NGAP Uplink Non-UE Associated NRPPa <br> (NRPPa TRP INFORMATION RESPONSE)
    AMF->>LMF: Namf_Comm_N2InfoNotify

    Note over UE, LMF: SRS Configuration

    LMF->>AMF: NRPPa POSITIONING INFORMATION REQUEST
    AMF->>CU: NGAP Downlink UE Associated NRPPa <br> (NRPPa POSITIONING INFORMATION REQUEST)
    
    CU->>DU: F1AP POSITIONING INFORMATION REQUEST
    Note right of DU: DU allocates SRS Config<br/>(Comb, Shift, Periodicity)
    DU->>CU: F1AP POSITIONING INFORMATION RESPONSE
    
    CU->>AMF: NGAP Uplink UE Associated NRPPa <br> (NRPPa POSITIONING INFORMATION RESPONSE)
    AMF->>LMF: Namf_Comm_N2InfoNotify

    Note over UE, LMF: Activation
    
    LMF->>AMF: NRPPa POSITIONING ACTIVATION REQUEST
    AMF->>CU: NGAP Downlink UE Associated NRPPa <br> (NRPPa POSITIONING ACTIVATION REQUEST)
    
    CU->>DU: F1AP POSITIONING ACTIVATION REQUEST
    Note right of DU: DU look for SRS (periodic)
    DU->>CU: F1AP POSITIONING ACTIVATION RESPONSE
    
    CU->>AMF: NGAP Uplink UE Associated NRPPa <br> (NRPPa POSITIONING ACTIVATION RESPONSE)
    AMF->>LMF: Namf_Comm_N2InfoNotify

    Note over UE, LMF: Measurement (Distributed TRPs)
    
    LMF->>AMF: NRPPa MEASUREMENT REQUEST
    Note right of LMF: Request: Measure UL-RTOA on TRP 1 & TRP 2
    AMF->>CU: NGAP Downlink UE Associated NRPPa <br> (NRPPa MEASUREMENT REQUEST)
    
    CU->>DU: F1AP POSITIONING MEASUREMENT REQUEST <br> (List: TRP 1, TRP 2)
    Note right of DU: DU already manages the UE Context<br/>and inherently knows the SRS config to use for both TRPs

    Note right of UE: UE Transmits SRS
    UE->>TRP1: UL SRS Transmission
    UE->>TRP2: UL SRS Transmission
    
    TRP1-->>DU: IQ Samples / Baseband Timing
    TRP2-->>DU: IQ Samples / Baseband Timing
    
    Note right of DU: DU calculates UL-RTOA for TRP 1 & TRP 2
    DU->>CU: F1AP POSITIONING MEASUREMENT REPORT <br> (UL-RTOAs for TRP 1 & TRP 2)
    
    CU->>AMF: NGAP Uplink UE Associated NRPPa <br> (NRPPa MEASUREMENT RESPONSE)
    AMF->>LMF: Namf_Comm_N2InfoNotify

    Note over UE, LMF: Calculation
    Note right of LMF: LMF computes TDOA:<br/>(RTOA_TRP2 - RTOA_TRP1)
```
