// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gnb_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "gnb_appconfig.h"

#ifdef JBPF_ENABLED
#include "jbpf.h"
#include "jbpf_srsran_defs.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

using namespace ocudu;
using namespace std::chrono_literals;

void ocudu::fill_gnb_worker_manager_config(worker_manager_config& config, const gnb_appconfig& app_cfg)
{
  ocudu_assert(config.cu_up_cfg, "CU-UP worker config does not exist");
  ocudu_assert(config.du_hi_cfg, "DU high worker config does not exist");

  config.nof_main_pool_threads     = app_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = app_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{app_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = app_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}

#ifdef JBPF_ENABLED
void ocudu::generate_jbpf_config(const gnb_appconfig& config, struct jbpf_config* jbpf_cfg)
{
  strncpy(jbpf_cfg->jbpf_run_path, config.jbpf_cfg.jbpf_run_path.c_str(), JBPF_RUN_PATH_LEN - 1);
  strncpy(jbpf_cfg->jbpf_namespace, config.jbpf_cfg.jbpf_namespace.c_str(), JBPF_NAMESPACE_LEN - 1);
  if (config.jbpf_cfg.jbpf_ipc_enabled == 0) {
    jbpf_cfg->io_config.io_type                      = JBPF_IO_THREAD_CONFIG;
    jbpf_cfg->io_config.io_thread_config.io_mem_size = config.jbpf_cfg.jbpf_io_mem_size_mb * 1024;
    if (config.jbpf_cfg.jbpf_standalone_io_cpu != 0) {
      jbpf_cfg->io_config.io_thread_config.has_affinity_io_thread   = 1;
      jbpf_cfg->io_config.io_thread_config.io_thread_affinity_cores = config.jbpf_cfg.jbpf_standalone_io_cpu;
    }
    if (config.jbpf_cfg.jbpf_standalone_io_priority != 0) {
      jbpf_cfg->io_config.io_thread_config.has_sched_priority_io_thread = 1;
      jbpf_cfg->io_config.io_thread_config.io_thread_sched_priority     = config.jbpf_cfg.jbpf_standalone_io_priority;
    }
    jbpf_cfg->io_config.io_thread_config.has_sched_policy_io_thread = 1;
    jbpf_cfg->io_config.io_thread_config.io_thread_sched_policy     = config.jbpf_cfg.jbpf_standalone_io_policy;

    output_socket* sock = new output_socket();
    // Create a UDP socket to send messages out
    sock->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&sock->server_addr, 0, sizeof(sock->server_addr));
    sock->server_addr.sin_family      = AF_INET;
    sock->server_addr.sin_port        = htons(config.jbpf_cfg.jbpf_standalone_io_out_port);
    sock->server_addr.sin_addr.s_addr = inet_addr(config.jbpf_cfg.jbpf_standalone_io_out_ip.c_str());
    std::cout << "[JANUS_INFO] Creating UDP connection to " << config.jbpf_cfg.jbpf_standalone_io_out_ip.c_str() << ":"
              << config.jbpf_cfg.jbpf_standalone_io_out_port << std::endl;
    jbpf_cfg->io_config.io_thread_config.output_handler_ctx = sock;
  } else if (config.jbpf_cfg.jbpf_ipc_enabled == 1) {
    jbpf_cfg->io_config.io_type = JBPF_IO_IPC_CONFIG;
    strncpy(jbpf_cfg->io_config.io_ipc_config.ipc_name, config.jbpf_cfg.jbpf_ipc_mem_name.c_str(), MAX_IPC_NAME_LEN - 1);
    jbpf_cfg->io_config.io_ipc_config.ipc_mem_size = config.jbpf_cfg.jbpf_io_mem_size_mb * 1024 * 1024;
  }

  if (config.jbpf_cfg.jbpf_has_lcm_ipc_thread) {
    strncpy(jbpf_cfg->lcm_ipc_config.lcm_ipc_name, config.jbpf_cfg.jbpf_lcm_ipc_name.c_str(), JBPF_LCM_IPC_NAME_LEN - 1);
  }

  if (config.jbpf_cfg.jbpf_agent_cpu != 0) {
    jbpf_cfg->thread_config.has_affinity_agent_thread   = 1;
    jbpf_cfg->thread_config.agent_thread_affinity_cores = config.jbpf_cfg.jbpf_agent_cpu;
  }
  jbpf_cfg->thread_config.has_sched_policy_agent_thread = 1;
  jbpf_cfg->thread_config.agent_thread_sched_policy     = config.jbpf_cfg.jbpf_agent_policy;

  if (config.jbpf_cfg.jbpf_agent_priority != 0) {
    jbpf_cfg->thread_config.has_sched_priority_agent_thread = 1;
    jbpf_cfg->thread_config.agent_thread_sched_priority     = config.jbpf_cfg.jbpf_agent_priority;
  }
  if (config.jbpf_cfg.jbpf_maint_cpu != 0) {
    jbpf_cfg->thread_config.has_affinity_maintenance_thread   = 1;
    jbpf_cfg->thread_config.maintenance_thread_affinity_cores = config.jbpf_cfg.jbpf_maint_cpu;
  }
  jbpf_cfg->thread_config.has_sched_policy_maintenance_thread = 1;
  jbpf_cfg->thread_config.maintenance_thread_sched_policy     = config.jbpf_cfg.jbpf_maint_policy;
  if (config.jbpf_cfg.jbpf_maint_priority != 0) {
    jbpf_cfg->thread_config.has_sched_priority_maintenance_thread = 1;
    jbpf_cfg->thread_config.maintenance_thread_sched_priority     = config.jbpf_cfg.jbpf_maint_priority;
  }
}
#endif
