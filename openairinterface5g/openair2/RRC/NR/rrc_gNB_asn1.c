/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdbool.h>
#include "NR_DRB-ToAddMod.h"
#include "NR_RadioBearerConfig.h"
#include "common/utils/oai_asn1.h"
#include "common/utils/LOG/log.h"
#include "openair2/LAYER2/nr_pdcp/nr_pdcp_configuration.h"
#include "openair2/LAYER2/nr_pdcp/nr_pdcp_asn1_utils.h"

NR_PDCP_Config_t *nr_rrc_build_pdcp_config_ie(const bool integrity, const bool ciphering, const nr_pdcp_configuration_t *pdcp)
{
  NR_PDCP_Config_t *out = calloc_or_fail(1, sizeof(*out));
  asn1cCallocOne(out->t_Reordering, encode_t_reordering(pdcp->drb.t_reordering));
  if (!ciphering) {
    asn1cCalloc(out->ext1, ext1);
    asn1cCallocOne(ext1->cipheringDisabled, NR_PDCP_Config__ext1__cipheringDisabled_true);
  }
  asn1cCalloc(out->drb, drb);
  asn1cCallocOne(drb->discardTimer, encode_discard_timer(pdcp->drb.discard_timer));
  asn1cCallocOne(drb->pdcp_SN_SizeUL, encode_sn_size_ul(pdcp->drb.sn_size));
  asn1cCallocOne(drb->pdcp_SN_SizeDL, encode_sn_size_dl(pdcp->drb.sn_size));
  drb->headerCompression.present = NR_PDCP_Config__drb__headerCompression_PR_notUsed;
  drb->headerCompression.choice.notUsed = 0;
  if (integrity) {
    asn1cCallocOne(drb->integrityProtection, NR_PDCP_Config__drb__integrityProtection_enabled);
  }
  return out;
}
