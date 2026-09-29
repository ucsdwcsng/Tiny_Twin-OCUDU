<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# The gNB RACH Processing

## 1. Collect I/Q Samples

The gNB/ru main loop, in `ru_thread()`
If there is rach occasions in this slot
this main loop calls `rx_nr_prach_ru()` that stores the I/Q samples in the `prach_item_t` context.

## 2. Process the Samples

`L1_rx_thread()` has received slot indication by msg on queue `resp_L1`, it calls `rx_func() => L1_nr_prach_procedures()`
`L1_nr_prach_procedures()` fills `nfapi_nr_rach_indication_t` structure from result of `rx_nr_prach()` that process the content of `rxsigF[prachOccasion][aa]`
`rx_func()` sends the decoded rach to upper processing by call to `NR_UL_indication()` that calls `handle_nr_rach() => nr_initiate_ra_proc()`.

`nr_initiate_ra_proc()` creates a UE context if it needs, then sets `UE.ra.ra_state` to what it should do.

## 3. Send the RACH DL

`tx_func()` that is launched by `L1_tx_thread()` as it gets `L1_tx_out` msg (from main loop in [1.](#1-collect-iq-samples))
`tx_func()=>run_scheduler_monolithic()=>gNB_dlsch_ulsch_scheduler()=>nr_scheduler_RA()`
`nr_scheduler_RA()` process all existing UE contexts, to see it it has to schedule a RACH DL.

## 4. Global Synchronisation

`resp_L1` msg for ([2.](#2-process-the-samples)) is created in `tx_func()` that is launched by `L1_tx_thread()` (see [3.](#3-send-the-rach-dl)) when it receives slot indication as it gets `L1_tx_out` msg
this msg is sent by the main loop (see [1.](#1-collect-iq-samples))
`tx_func()=>run_scheduler_monolithic()=>nr_schedule_response ()=>nr_schedule_ul_tti_req ()=> nr_schedule_rx_prach()`
writes in the triggering structure to ([1.](#1-collect-iq-samples)) that it will have to run  `rx_nr_prach_ru()` in a given future slot.
