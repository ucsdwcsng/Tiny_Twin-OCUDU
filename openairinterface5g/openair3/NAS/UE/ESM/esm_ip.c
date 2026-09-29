/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "emmData.h"
#include "esmData.h"

char *esm_data_get_ipv4_addr(const OctetString *ip_addr, char *ret)
{
  if (ip_addr->length > 0) {
    sprintf(ret, "%u.%u.%u.%u",
            ip_addr->value[0], ip_addr->value[1],
            ip_addr->value[2], ip_addr->value[3]);
    return ret;
  }

  return (NULL);
}

char *esm_data_get_ipv6_addr(const OctetString *ip_addr, char *ret)
{
  if (ip_addr->length > 0) {
    sprintf(ret, "%x%.2x:%x%.2x:%x%.2x:%x%.2x",
            ip_addr->value[0], ip_addr->value[1],
            ip_addr->value[2], ip_addr->value[3],
            ip_addr->value[4], ip_addr->value[5],
            ip_addr->value[6], ip_addr->value[7]);
    return ret;
  }

  return (NULL);
}

char *esm_data_get_ipv4v6_addr(const OctetString *ip_addr, char *ret)
{
  if (ip_addr->length > 0) {
    sprintf(ret, "%u.%u.%u.%u / %x%.2x:%x%.2x:%x%.2x:%x%.2x",
            ip_addr->value[0], ip_addr->value[1],
            ip_addr->value[2], ip_addr->value[3],
            ip_addr->value[4], ip_addr->value[5],
            ip_addr->value[6], ip_addr->value[7],
            ip_addr->value[8], ip_addr->value[9],
            ip_addr->value[10], ip_addr->value[11]);
    return ret;
  }

  return (NULL);
}
