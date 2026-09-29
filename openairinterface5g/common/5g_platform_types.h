/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FIVEG_PLATFORM_TYPES_H__
#define FIVEG_PLATFORM_TYPES_H__

#include <stdint.h>

typedef struct plmn_id_s {
  uint16_t mcc;
  uint16_t mnc;
  uint8_t mnc_digit_length;
} plmn_id_t;

typedef struct nssai_s {
  uint8_t sst;
  uint32_t sd;
} nssai_t;

// Globally Unique AMF Identifier
typedef struct nr_guami_s {
  plmn_id_t plmn;
  uint8_t amf_region_id;
  uint16_t amf_set_id;
  uint8_t amf_pointer;
} nr_guami_t;

typedef enum {
  PDUSessionType_ipv4 = 0,
  PDUSessionType_ipv6 = 1,
  PDUSessionType_ipv4v6 = 2,
  PDUSessionType_ethernet = 3,
  PDUSessionType_unstructured = 4
} pdu_session_type_t;

typedef enum { NON_DYNAMIC, DYNAMIC } fiveQI_t;

/* 5QI (5G QoS Identifier) - 3GPP TS 23.501 §5.7.2.1
 * Range: 0..255
 * - Standardized 5QI values: have one-to-one mapping to standardized 5G QoS characteristics (Table 5.7.4-1)
 * - Pre-configured 5QI values: pre-configured in the AN (not in Table 5.7.4-1)
 * - Dynamically assigned 5QI values: require signaling of QoS characteristics as part of QoS profile
 * OAI implements standardized non-dynamic 5QI only. */
#define MIN_FIVEQI 0
#define MAX_STANDARDIZED_FIVEQI 90
#define MAX_FIVEQI 255

/* ARP Priority Level - 3GPP TS 23.501 §5.7.2.2
 * The ARP priority level defines the relative importance of a QoS Flow.
 * Range: 1 to 15, with 1 as the highest priority.
 * ARP priority levels 1-8: authorized by serving network (prioritized treatment)
 * ARP priority levels 9-15: authorized by home network (roaming scenarios) */
typedef uint8_t qos_arp_priority_level_t;
#define MIN_QOS_ARP_PRIORITY_LEVEL 1 // highest priority
#define MAX_QOS_ARP_PRIORITY_LEVEL 15 // lowest priority

/* Pre-emption Capability */
typedef enum {
  PEC_SHALL_NOT_TRIGGER_PREEMPTION = 0,
  PEC_MAY_TRIGGER_PREEMPTION,
  PEC_MAX,
} qos_pec_t;

/* Pre-emption Vulnerability */
typedef enum {
  PEV_NOT_PREEMPTABLE = 0,
  PEV_PREEMPTABLE = 1,
  PEV_MAX,
} qos_pev_t;

/* Allocation and Retention Priority (ARP) - 3GPP TS 23.501 §5.7.2.2
 * Contains information about priority level, pre-emption capability and vulnerability.
 * Used for admission control of GBR traffic and pre-emption decisions. */
typedef struct {
  // ARP priority level (1-15, 1 = highest)
  qos_arp_priority_level_t priority_level;
  qos_pec_t pre_emp_capability;
  qos_pev_t pre_emp_vulnerability;
} qos_arp_t;

/* QoS Priority Level - 3GPP TS 23.501 §5.7.3.3
 * The Priority Level associated with 5G QoS characteristics indicates a priority
 * in scheduling resources among QoS Flows. The lowest Priority Level value
 * corresponds to the highest priority.
 * Range: 1 to 127, with 1 as the highest priority and 127 as the lowest priority.
 * Used for scheduling resources among QoS Flows (different from ARP priority level
 * which is used for admission control/preemption).
 * Every standardized 5QI is associated with a default Priority Level value. */
typedef uint8_t qos_priority_level_t;
#define MIN_QOS_PRIORITY_LEVEL 1 // highest priority
#define MAX_QOS_PRIORITY_LEVEL 127 // lowest priority

/* Packet Delay Budget (PDB) - 3GPP TS 23.501 §5.7.3.4
 * Upper bound for packet delay between UE and UPF N6 termination point (0..1023 ms).
 * Used for scheduling and link layer configuration (e.g. HARQ target operating points).
 * For Delay-critical GBR flows, packets delayed more than PDB are counted as lost. */
#define MIN_PACKET_DELAY_BUDGET 0
#define MAX_PACKET_DELAY_BUDGET 1023

/* Packet Error Rate (PER) - 3GPP TS 23.501 §5.7.3.5
 * Upper bound for rate of non-congestion related packet losses (0..9 for scalar/exponent).
 * Used for link layer protocol configuration (e.g. RLC, HARQ).
 * PER = Scalar * 10^(-Exponent) */
#define MIN_PACKET_ERROR_RATE_SCALAR 0
#define MAX_PACKET_ERROR_RATE_SCALAR 9
#define MIN_PACKET_ERROR_RATE_EXPONENT 0
#define MAX_PACKET_ERROR_RATE_EXPONENT 9

typedef struct {
  // Packet Error Rate Scalar (0..9)
  uint8_t scalar;
  // Packet Error Rate Exponent (0..9)
  uint8_t exponent;
} qos_per_t;

/** QoS Characteristics for a standardized or
 * pre-configured 5QI for downlink and uplink*/
typedef struct {
  uint16_t fiveQI;
  qos_priority_level_t *qos_priority;
} non_dynamic_5qi_t;

/** QoS Characteristics for a Non-standardised or
 * not pre-configured 5QI for downlink and uplink. */
typedef struct {
  uint16_t *fiveQI;
  qos_priority_level_t qos_priority;
  // Packet Delay Budget (0..1023 ms)
  int packet_delay_budget;
  // Packet Error Rate (0..9 for scalar/exponent)
  qos_per_t per;
} dynamic_5qi_t;

/* Bit Rate (kbps) - 3GPP TS 38.413 */
typedef struct qos_bitrate_s {
  // Guaranteed Flow Bit Rate (GFBR) (kbps)
  uint64_t guaranteedFlowBitRate;
  // Maximum Flow Bit Rate (MFBR) (kbps)
  uint64_t maximumFlowBitRate;
} qos_bitrate_t;

/* GBR QoS Flow Information - 3GPP TS 23.501 §5.7.1.2, TS 38.413 §9.3.1.19
 * Present only for GBR QoS flows (5QI < 5 for NonDynamic5QI, or Dynamic5QI with GBR).
 * For GBR QoS Flow only, the QoS profile SHALL include for DL and UL:
 * - Guaranteed Flow Bit Rate (GFBR)
 * - Maximum Flow Bit Rate (MFBR) */
typedef struct gbr_qos_flow_information_s {
  // Downlink bit rates (kbps)
  qos_bitrate_t dl;
  // Uplink bit rates (kbps)
  qos_bitrate_t ul;
} gbr_qos_flow_information_t;

typedef struct pdusession_level_qos_parameter_s {
  uint8_t qfi;
  // QoS Characteristics
  fiveQI_t fiveQI_type;
  union {
    non_dynamic_5qi_t non_dynamic;
    dynamic_5qi_t dynamic;
  } qos_characteristics;
  // NG-RAN Allocation and Retention Priority
  qos_arp_t arp;
  /* GBR QoS Flow Information (optional - only for GBR flows) */
  gbr_qos_flow_information_t *gbr_qos_flow_information;
} pdusession_level_qos_parameter_t;

#endif
