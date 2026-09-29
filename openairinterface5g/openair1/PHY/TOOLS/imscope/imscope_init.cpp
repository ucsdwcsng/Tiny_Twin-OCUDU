/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "imscope_internal.h"

extern "C" void imscope_autoinit(void *dataptr)
{
  AssertFatal(IS_SOFTMODEM_5GUE || IS_SOFTMODEM_GNB,
              "Scope cannot find NRUE or GNB context");

  for (auto i = 0U; i < EXTRA_SCOPE_TYPES; i++) {
    scope_array[i].is_data_ready = false;
    scope_array[i].data.scope_graph_data = nullptr;
    scope_array[i].data.meta = {-1, -1};
  }

  scopeData_t *scope = (scopeData_t *)calloc(1, sizeof(scopeData_t));
  scope->copyData = copyDataThreadSafe;
  scope->tryLockScopeData = tryLockScopeData;
  scope->copyDataUnsafeWithOffset = copyDataUnsafeWithOffset;
  scope->unlockScopeData = unlockScopeData;
  scope->copyData = copyDataThreadSafe;
  if (IS_SOFTMODEM_GNB) {
    scopeParms_t *scope_params = (scopeParms_t *)dataptr;
    scope_params->gNB->scopeData = scope;
    if (scope_params->ru)
      scope_params->ru->scopeData = scope;
  } else {
    PHY_VARS_NR_UE *ue = (PHY_VARS_NR_UE *)dataptr;
    ue->scopeData = scope;
  }

  pthread_t thread;
  threadCreate(&thread, imscope_thread, dataptr, (char *)"imscope", -1, sched_get_priority_min(SCHED_RR));
}
