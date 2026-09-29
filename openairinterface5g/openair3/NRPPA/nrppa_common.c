/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nrppa_common.h"

NRPPA_Cause_t encode_nrppa_cause(nrppa_cause_t cause)
{
  NRPPA_Cause_t nrppa_cause = {0};
  switch (cause.type) {
    case NRPPA_CAUSE_RADIO_NETWORK:
      nrppa_cause.present = NRPPA_Cause_PR_radioNetwork;
      nrppa_cause.choice.radioNetwork = cause.value;
      break;
    case NRPPA_CAUSE_PROTOCOL:
      nrppa_cause.present = NRPPA_Cause_PR_protocol;
      nrppa_cause.choice.protocol = cause.value;
      break;
    case NRPPA_CAUSE_MISC:
      nrppa_cause.present = NRPPA_Cause_PR_misc;
      nrppa_cause.choice.misc = cause.value;
      break;
    case NRPPA_CAUSE_NOTHING:
    default:
      AssertFatal(false, "unknown cause value %d\n", cause.type);
      break;
  }
  return nrppa_cause;
}
