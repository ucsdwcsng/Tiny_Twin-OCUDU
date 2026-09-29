#pragma once

/* OFH context*/
// struct jbpf_ran_ofh_ctx {
//   uint64_t data; /* Pointer to beginning of buffer with int16_t IQ samples */
//   uint64_t data_end; /* Pointer to end+1 of packet */
//   uint64_t meta_data;/* Used by ebpf */
//   /* Combination of frame, slot and cell_id, provide a unique
//      context for an execution pipeline */
//   uint16_t ctx_id; /* Context id (could be implementation specific) */
//   JbpfDirection_t direction; /* 0 DL, 1 UL */
// };
constexpr int jbpf_ran_ofh_regions = 3 * 8 + 1 * 2 + 1;
constexpr ebpf_context_descriptor_t jbpf_ran_ofh_descr = {jbpf_ran_ofh_regions, 0, 1 * 8, 2 * 8};

/* L2 context*/
// struct jbpf_ran_layer2_ctx {
//     uint64_t data; /* Pointer to beginning of variable-sized L2 struct */
//     uint64_t data_end; /* Pointer to end+1 of variable-sized struct */
//     uint64_t meta_data; /* Used by ebpf */
//     uint16_t ctx_id; /* Context id (could be implementation specific) */
//     uint16_t frame; /* 3GPP frame number */
//     uint16_t slot; /* 3GPP slot number */
//     uint16_t cell_id; /* Cell id */
// };
constexpr int jbpf_ran_layer2_regions = 3 * 8 + 4 * 2;
constexpr ebpf_context_descriptor_t jbpf_ran_layer2_descr = {jbpf_ran_layer2_regions, 0, 1 * 8, 2 * 8};


/* MAC Scheduler context */
// struct jbpf_mac_sched_ctx {
//     uint64_t data; /* Pointer to beginning of variable-sized L2 struct */
//     uint64_t data_end; /* Pointer to end+1 of variable-sized struct */
//     uint64_t meta_data; /* Used by ebpf */
//     uint64_t srs_meta_data1; /* Used for the program to store metadata */
//     uint16_t ctx_id; /* Context id (could be implementation specific) */
//     uint16_t ue_index; /* UE index */
//     uint16_t cell_id; /* Cell id */
//     uint16_t rnti; /* UE RNTI */
// };
constexpr int jbpf_ran_mac_sched_regions = 4 * 8 + 4 * 2;
constexpr ebpf_context_descriptor_t jbpf_ran_mac_sched_descr = {jbpf_ran_mac_sched_regions, 0, 1 * 8, 2 * 8};

// struct jbpf_ran_generic_ctx {
//    uint64_t data; /* Pointer to beginning of buffer with int16_t IQ samples */
//    uint64_t data_end; /* Pointer to end+1 of packet */
//    uint64_t meta_data; /* Used by ebpf */
//    uint64_t srs_meta_data1; /* Used for the program to store metadata */
//    uint64_t srs_meta_data2; /* Used for the program to store metadata */
//    uint64_t srs_meta_data3; /* Used for the program to store metadata */
//    uint64_t srs_meta_data4; /* Used for the program to store metadata */
// };
constexpr int jbpf_srsran_generic_regions = 7 * 8 ;
constexpr ebpf_context_descriptor_t jbpf_srsran_generic_descr = {jbpf_srsran_generic_regions, 0, 1 * 8, 2 * 8};
