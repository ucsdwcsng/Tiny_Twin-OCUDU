/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "get-yang.h"
#include "../rpc-send-recv.h"
#include "common/utils/assertions.h"

#include <libxml/parser.h>

static bool store_schemas(xmlNode *node, ru_session_t *ru_session, struct ly_ctx **ctx)
{
  int timeout = CLI_RPC_REPLY_TIMEOUT;
  NC_WD_MODE wd = NC_WD_ALL;
  NC_PARAMTYPE param = NC_PARAMTYPE_CONST;
  LYS_INFORMAT ly_format = LYS_IN_YANG;

#ifdef MPLANE_V1
  *ctx = ly_ctx_new(NULL, 0);
#elif defined MPLANE_V2
  ly_ctx_new(NULL, 0, ctx);
#endif

  for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
    if (strcmp((const char *)cur_node->name, "schema") == 0) {

      xmlNode *name_node = xmlFirstElementChild(cur_node);
      xmlChar *module_name = xmlNodeGetContent(name_node);
      xmlNode *revision_node = xmlNextElementSibling(name_node);
      xmlChar *module_revision = xmlNodeGetContent(revision_node);
      xmlNode *format_node = xmlNextElementSibling(revision_node);
      xmlChar *module_format = xmlNodeGetContent(format_node);

      if (strcmp((char*)module_format, "yang") != 0) {
	xmlFree(module_name);
	xmlFree(module_revision);
	xmlFree(module_format);
        continue;
      }

      MP_LOG_I("RPC request to RU \"%s\" = <get-schema> for module \"%s\".\n", ru_session->ru_ip_add, module_name);
      struct nc_rpc *get_schema_rpc = nc_rpc_getschema((char*)module_name, (char*)module_revision, "yang", param);
      char *schema_data = NULL;
      bool success = rpc_send_recv((struct nc_session *)ru_session->session, get_schema_rpc, wd, timeout, &schema_data);
      if (!success) {
        MP_LOG_W("[MPLANE] Unable to get schema for module \"%s\" from RU \"%s\".\n", module_name, ru_session->ru_ip_add);
        xmlFree(module_name);
        xmlFree(module_revision);
        xmlFree(module_format);
	return false;
      }

      if (schema_data) {
#ifdef MPLANE_V1
        const struct lys_module *mod = lys_parse_mem(*ctx, schema_data, ly_format);
#elif defined MPLANE_V2
        struct lys_module *mod = NULL;
        lys_parse_mem(*ctx, schema_data, ly_format, &mod);
#endif
        free(schema_data);
        if (!mod) {
          MP_LOG_W("Unable to load module \"%s\" from RU \"%s\".\n", module_name, ru_session->ru_ip_add);
	        nc_rpc_free(get_schema_rpc);
#ifdef MPLANE_V1
          ly_ctx_destroy(*ctx, NULL);
#elif defined MPLANE_V2
          ly_ctx_destroy(*ctx);
#endif
	  xmlFree(module_name);
          xmlFree(module_revision);
          xmlFree(module_format);
	  return false;
        }
      }
      nc_rpc_free(get_schema_rpc);
      xmlFree(module_name);
      xmlFree(module_revision);
      xmlFree(module_format);
    }
  }

  return true;
}

static bool load_from_operational_ds(xmlNode *node, ru_session_t *ru_session, struct ly_ctx **ctx)
{
  for (xmlNode *schemas_node = node; schemas_node; schemas_node = schemas_node->next) {
    if(schemas_node->type != XML_ELEMENT_NODE)
      continue;
    if (strcmp((const char *)schemas_node->name, "schemas") == 0) {
      return store_schemas(schemas_node->children, ru_session, ctx);
    } else {
      bool success = load_from_operational_ds(schemas_node->children, ru_session, ctx);
      if (success)
        return true;
    }
  }
  return false;
}

bool load_yang_models(ru_session_t *ru_session, const char *buffer)
{
  struct ly_ctx **ctx = (struct ly_ctx **)&ru_session->ctx;

  // Initialize the xml file
  size_t len = strlen(buffer) + 1;
  xmlDoc *doc = xmlReadMemory(buffer, len, NULL, NULL, 0);
  xmlNode *root_element = xmlDocGetRootElement(doc);

  bool success = load_from_operational_ds(root_element->children, ru_session, ctx);
  xmlFreeDoc(doc);
  if (success) {
    MP_LOG_I("Successfully loaded all yang modules from operational datastore for RU \"%s\".\n", ru_session->ru_ip_add);
    return true;
  } else {
    MP_LOG_W("Unable to load all yang modules from operational datastore for RU \"%s\". Using yang models present in \"models\" subfolder.\n", ru_session->ru_ip_add);
  }

  /* ideally, we should load the yang models from the operational datastore, but the issues are following:
    1) the yang models order is not good - the dependancy models have to be loaded first
    2) earlier O-RAN yang versions (e.g. v4) is not properly defined (i.e. optional parameters should not be included by default) */
  const char *yang_dir = YANG_MODELS;
  const char *yang_models[] = {"ietf-interfaces", "iana-if-type", "ietf-ip", "iana-hardware", "ietf-hardware", "o-ran-wg4-features", "o-ran-interfaces", "o-ran-module-cap", "o-ran-compression-factors", "ietf-crypto-types", "o-ran-usermgmt", "o-ran-processing-element", "o-ran-hardware", "o-ran-common-yang-types", "o-ran-delay-management", "o-ran-uplane-conf", "ietf-netconf-acm", "o-ran-file-management", "o-ran-performance-management"};

#ifdef MPLANE_V1
  *ctx = ly_ctx_new(yang_dir, 0);
  AssertError(*ctx != NULL, return false, "[MPLANE] Unable to create libyang context: %s.\n", ly_errmsg(*ctx));
#elif defined MPLANE_V2
  LY_ERR ret = ly_ctx_new(yang_dir, 0, ctx);
  AssertError(ret == LY_SUCCESS, return false, "[MPLANE] Unable to create libyang context: %s.\n", ly_errmsg(*ctx));
#endif

  const size_t num_models = sizeof(yang_models) / sizeof(yang_models[0]);
  for (size_t i = 0; i < num_models; ++i) {
#ifdef MPLANE_V1
    const struct lys_module *mod = ly_ctx_load_module(*ctx, yang_models[i], NULL);
    AssertError(mod != NULL, return false, "[MPLANE] Failed to load yang model %s.\n", yang_models[i]);
#elif defined MPLANE_V2
    const struct lys_module *mod = ly_ctx_load_module(*ctx, yang_models[i], NULL, NULL);
    AssertError(mod != NULL, return false, "[MPLANE] Failed to load yang model %s.\n", yang_models[i]);
#endif
  }

  MP_LOG_I("Successfully loaded all yang modules for RU \"%s\".\n", ru_session->ru_ip_add);

  return true;
}
