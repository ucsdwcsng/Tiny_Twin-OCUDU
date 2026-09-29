/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "f1ap_common.h"

static f1ap_cudu_inst_t *f1_inst[NUMBER_OF_gNB_MAX]= {0};

uint8_t F1AP_get_next_transaction_identifier(instance_t mod_idP, instance_t cu_mod_idP)
{
  DevAssert(mod_idP == 0 && cu_mod_idP == 0);
  static uint8_t transaction_identifier[NUMBER_OF_gNB_MAX] = {0};
  transaction_identifier[mod_idP+cu_mod_idP] =
    (transaction_identifier[mod_idP+cu_mod_idP] + 1) % F1AP_TRANSACTION_IDENTIFIER_NUMBER;
  return transaction_identifier[mod_idP+cu_mod_idP];
}

f1ap_cudu_inst_t *getCxt(instance_t instanceP)
{
  DevAssert(instanceP == 0);
  return f1_inst[instanceP];
}

static pthread_mutex_t f1_inst_mtx = PTHREAD_MUTEX_INITIALIZER;
void createF1inst(instance_t instanceP, f1ap_setup_req_t *req, f1ap_net_config_t *nc)
{
  DevAssert(instanceP == 0);
  pthread_mutex_lock(&f1_inst_mtx);
  AssertFatal(f1_inst[0] == NULL, "Attempted to initialize multiple F1 instances\n");
  f1_inst[0] = calloc(1, sizeof(f1ap_cudu_inst_t));
  AssertFatal(f1_inst[0] != NULL, "out of memory\n");
  if (req)
    f1_inst[0]->setupReq = *req;
  if (nc)
    f1_inst[0]->net_config = *nc;
  pthread_mutex_unlock(&f1_inst_mtx);
}

void destroyF1inst(instance_t instance)
{
  DevAssert(instance == 0);
  pthread_mutex_lock(&f1_inst_mtx);
  AssertFatal(f1_inst[0] != NULL, "Attempted to free uninitialized F1 instances\n");
  free(f1_inst[instance]);
  f1_inst[instance] = NULL;
  pthread_mutex_unlock(&f1_inst_mtx);
}
