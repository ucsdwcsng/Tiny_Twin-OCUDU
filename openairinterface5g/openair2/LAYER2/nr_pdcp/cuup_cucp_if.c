/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "cuup_cucp_if.h"

static e1_if_t e1_if;
static bool g_uses_e1;

e1_if_t *get_e1_if(void)
{
  return &e1_if;
}

bool e1_used(void)
{
  return g_uses_e1;
}

void nr_pdcp_e1_if_init(bool uses_e1)
{
  g_uses_e1 = uses_e1;

  if (uses_e1)
    cuup_cucp_init_e1ap(&e1_if);
  else
    cuup_cucp_init_direct(&e1_if);
}
