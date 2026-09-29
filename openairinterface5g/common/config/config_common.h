/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: configuration module, include file to be used by the source code calling the
 *  configuration module to access configuration parameters
 */

#ifndef INCLUDE_CONFIG_COMMON_H
#define INCLUDE_CONFIG_COMMON_H
#include "config_load_configmodule.h"
#include "common/utils/utils.h"

#ifdef __cplusplus
extern "C"
{
#endif
void config_check_valptr(configmodule_interface_t *cfg, paramdef_t *cfgoptions, int elt_sz, int nb_elt);
/* functions to set a parameter to its default value */
int config_setdefault_int(configmodule_interface_t *cfg, paramdef_t *cfgoptions, const char *prefix);
int config_setdefault_int64(configmodule_interface_t *cfg, paramdef_t *cfgoptions, const char *prefix);
int config_setdefault_intlist(configmodule_interface_t *cfg, paramdef_t *cfgoptions);
int config_setdefault_string(configmodule_interface_t *cfg, paramdef_t *cfgoptions, const char *prefix);
int config_setdefault_stringlist(configmodule_interface_t *cfg, paramdef_t *cfgoptions, const char *prefix);
int config_setdefault_double(configmodule_interface_t *cfg, paramdef_t *cfgoptions);
int config_setdefault_ipv4addr(configmodule_interface_t *cfg, paramdef_t *cfgoptions);
void *config_allocate_new(configmodule_interface_t *cfg, int sz, bool autoFree);
void config_assign_int(configmodule_interface_t *cfg, paramdef_t *cfgoptions, char *fullname, int val);
int config_common_getdefault(configmodule_interface_t *cfg, paramdef_t *cfgoption, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif
