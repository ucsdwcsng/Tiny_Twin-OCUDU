/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "e3_agent.h"
#include "config/e3_config.h"

// TODO replace pthreads with itti or use a faster way
// #include "intertask_interface.h"
// #include "create_tasks.h"
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include <libe3/c_api.h>

#include "common/utils/system.h"
#include "common/ran_context.h"
#include "common/utils/LOG/log.h"
#include "openair2/GNB_APP/gnb_paramdef.h"

e3_agent_global_t e3 = {0};

static void e2_e3_bridge(uint32_t dapp_id, uint32_t ran_function_id, const uint8_t *report_data, size_t report_size)
{
  LOG_D(E3AP, "Received dApp report for RAN function %u from dApp %u (%zu bytes)\n", ran_function_id, dapp_id, report_size);
#ifdef E2_AGENT
  if (!report_data && report_size > 0) {
    LOG_E(E3AP, "Invalid dApp report payload: report_data is NULL while report_size=%zu\n", report_size);
    return;
  }
  generate_e2_indication_from_e3_dapp_report(ran_function_id, dapp_id, report_size, report_data);
#else
  (void)report_data;
#endif
}

void on_dapp_status_changed(void)
{
  LOG_I(E3AP, "dApp status changed, triggering RIC Service Update\n");
#ifdef E2_AGENT
  notify_dapp_status_changed();
#endif
}

int e3_init()
{
  LOG_D(E3AP, "Read configuration\n");
  e3_cmdline_config_t *e3_cmdline_configs = (e3_cmdline_config_t *)calloc(1, sizeof(e3_cmdline_config_t));
  if (!e3_cmdline_configs) {
    LOG_E(E3AP, "Failed to allocate E3 cmdline config\n");
    return -1;
  }
  e3_readconfig(e3_cmdline_configs);

  /* Read gNB name to use as E3 agent identifier */
  char *gnb_name = NULL;
  paramdef_t name_param[] = {{GNB_CONFIG_STRING_GNB_NAME, NULL, 0, .strptr = &gnb_name, .defstrval = "OAI gNB", TYPE_STRING, 0}};
  config_get(config_get_if(), name_param, 1, GNB_CONFIG_STRING_GNB_LIST ".[0]");

  e3_config_t config = {0};
  config.ran_identifier = gnb_name ? gnb_name : "OAI gNB";
  config.log_level = g_log->log_component[E3AP].level;
  config.link_layer = e3_cmdline_configs->link_layer;
  config.transport_layer = e3_cmdline_configs->transport_layer;
  config.encoding = e3_cmdline_configs->encoding;
  config.setup_port = e3_cmdline_configs->setup_port;
  config.subscriber_port = e3_cmdline_configs->subscriber_port;
  config.publisher_port = e3_cmdline_configs->publisher_port;

  e3.agent = e3_agent_create_with_config(&config);
  free(e3_cmdline_configs);
  e3_cmdline_configs = NULL;
  if (!e3.agent) {
    LOG_E(E3AP, "Failed to create E3Agent with config\n");
    return -1;
  }

  // Initialize agent
  e3_error_t err = e3_agent_init(e3.agent);
  if (err != 0) {
    LOG_E(E3AP, "Failed to initialize E3Agent (err=%d)\n", err);
    e3_agent_destroy(e3.agent);
    e3.agent = NULL;
    return -1;
  }

  // Start agent
  err = e3_agent_start(e3.agent);
  if (err != 0) {
    LOG_E(E3AP, "Failed to start E3Agent (err=%d)\n", err);
    e3_agent_destroy(e3.agent);
    e3.agent = NULL;
    return -1;
  }

  err = e3_agent_set_dapp_report_handler(e3.agent, e2_e3_bridge);
  if (err != 0) {
    LOG_E(E3AP, "Failed to set dApp report handler (err=%d: %s)\n", err, e3_error_to_string(err));
    e3_agent_destroy(e3.agent);
    e3.agent = NULL;
    return -1;
  }

  err = e3_agent_set_dapp_status_changed_handler(e3.agent, on_dapp_status_changed);
  if (err != 0) {
    LOG_E(E3AP, "Failed to set dApp status changed handler (err=%d: %s)\n", err, e3_error_to_string(err));
    e3_agent_destroy(e3.agent);
    e3.agent = NULL;
    return -1;
  }

  // No concrete service models are registered by the framework itself; service
  // models (e.g. spectrum sensing) register themselves in their own modules.
  return 0;
}

int e3_destroy()
{
  // Stop and destroy the E3Agent if it exists
  if (e3.agent) {
    e3_agent_stop(e3.agent);
    e3_agent_destroy(e3.agent);
    e3.agent = NULL;
  }

  return 0;
}

int e3_send_xapp_control(uint32_t dapp_id, uint32_t ran_function_id, const uint8_t *data, size_t len)
{
  if (!e3.agent) {
    LOG_E(E3AP, "E3 agent not initialized: cannot send xApp control\n");
    return -1;
  }

  if (data == NULL && len > 0) {
    LOG_E(E3AP, "data is not initialized, but len > 0\n");
    return -1;
  }

  e3_error_t err = e3_agent_send_xapp_control(e3.agent, dapp_id, ran_function_id, data, len);
  if (err != E3_SUCCESS) {
    LOG_E(E3AP, "Failed to send xApp control to dApp %u for RAN function %u (err=%d)\n", dapp_id, ran_function_id, err);
    return -1;
  }
  return 0;
}

e3_dapp_subscription_map_t e3_get_dapp_subscription_map(void)
{
  e3_dapp_subscription_map_t map = {0};

  if (!e3.agent) {
    LOG_W(E3AP, "E3 agent not initialized: cannot query dApp subscriptions\n");
    return map;
  }

  size_t num_dapps = 0;
  uint32_t *dapp_ids = e3_agent_get_registered_dapps(e3.agent, &num_dapps);
  if (!dapp_ids || num_dapps == 0) {
    e3_agent_free_uint32_array(dapp_ids);
    return map;
  }

  map.dapps = calloc(num_dapps, sizeof(e3_dapp_info_t));
  if (!map.dapps) {
    LOG_E(E3AP, "Failed to allocate dApp subscription map\n");
    e3_agent_free_uint32_array(dapp_ids);
    return map;
  }
  map.num_dapps = num_dapps;

  for (size_t i = 0; i < num_dapps; i++) {
    map.dapps[i].dapp_id = dapp_ids[i];

    size_t num_subs = 0;
    uint32_t *subs = e3_agent_get_dapp_subscriptions(e3.agent, dapp_ids[i], &num_subs);

    if (subs && num_subs > 0) {
      map.dapps[i].e3_ran_func_ids = subs; // transfer ownership from libe3 malloc
      map.dapps[i].num_e3_ran_funcs = num_subs;
    } else {
      map.dapps[i].e3_ran_func_ids = NULL;
      map.dapps[i].num_e3_ran_funcs = 0;
      e3_agent_free_uint32_array(subs);
    }
  }

  e3_agent_free_uint32_array(dapp_ids);
  return map;
}

void e3_free_dapp_subscription_map(e3_dapp_subscription_map_t *map)
{
  if (!map || !map->dapps)
    return;

  for (size_t i = 0; i < map->num_dapps; i++) {
    e3_agent_free_uint32_array(map->dapps[i].e3_ran_func_ids);
  }
  free(map->dapps);

  map->dapps = NULL;
  map->num_dapps = 0;
}
