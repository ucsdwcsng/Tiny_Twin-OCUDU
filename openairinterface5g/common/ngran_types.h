/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Definitions for NGRAN node types
 */

#ifndef __NGRAN_TYPES_H__
#define __NGRAN_TYPES_H__

typedef enum {
  ngran_eNB       = 0,
  ngran_ng_eNB    = 1,
  ngran_gNB       = 2,
  ngran_eNB_CU    = 3,
  ngran_ng_eNB_CU = 4,
  ngran_gNB_CU    = 5,
  ngran_eNB_DU    = 6,
  ngran_gNB_DU    = 7,
  ngran_eNB_MBMS_STA  = 8,
  ngran_gNB_CUCP  = 9,
  ngran_gNB_CUUP  = 10
} ngran_node_t;

typedef enum { CPtype = 0, UPtype } E1_t;

#define NODE_IS_MONOLITHIC(nOdE_TyPe) ((nOdE_TyPe) == ngran_eNB    || (nOdE_TyPe) == ngran_ng_eNB    || (nOdE_TyPe) == ngran_gNB)
#define NODE_IS_CU(nOdE_TyPe)         ((nOdE_TyPe) == ngran_eNB_CU || (nOdE_TyPe) == ngran_ng_eNB_CU || (nOdE_TyPe) == ngran_gNB_CU || (nOdE_TyPe) == ngran_gNB_CUCP || (nOdE_TyPe) == ngran_gNB_CUUP)
#define NODE_IS_DU(nOdE_TyPe)         ((nOdE_TyPe) == ngran_eNB_DU || (nOdE_TyPe) == ngran_gNB_DU)
#define NODE_IS_MBMS(nOdE_TyPe)       ((nOdE_TyPe) == ngran_eNB_MBMS_STA)
#define GTPV1_U_PORT_NUMBER (2152)

#endif
