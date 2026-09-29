// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/helpers/hal/hal_appconfig.h"
#include "apps/helpers/logger/logger_appconfig.h"
#include "apps/helpers/tracing/tracer_appconfig.h"
#include "apps/services/app_execution_metrics/executor_metrics_config.h"
#include "apps/services/app_resource_usage/app_resource_usage_config.h"
#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/metrics/metrics_appconfig.h"
#include "apps/services/remote_control/remote_control_appconfig.h"
#include "apps/services/worker_manager/worker_manager_appconfig.h"
#include "ocudu/ran/gnb_id.h"
#include <string>

#ifdef JBPF_ENABLED
#include "jbpf.h"
#endif

namespace ocudu {

struct cu_up_appconfig {
  bool warn_on_drop = false;
};

/// Metrics report configuration.
struct metrics_appconfig {
  app_services::app_resource_usage_config rusage_config;
  app_services::metrics_appconfig         metrics_service_cfg;
  app_services::executor_metrics_config   executors_metrics_cfg;
  bool                                    autostart_stdout_metrics = false;
};

#ifdef JBPF_ENABLED
/// Janus / jbpf configuration.
struct jbpf_appconfig {
  /// jbpf run path
  std::string jbpf_run_path = JBPF_DEFAULT_RUN_PATH;
  /// jbpf namespace
  std::string jbpf_namespace = JBPF_DEFAULT_NAMESPACE;
  /// jbpf mode (0 standalone, 1 ipc)
  unsigned jbpf_ipc_enabled = 0;
  /// LCM IPC thread for codelet lifecycle management
  bool jbpf_has_lcm_ipc_thread = true;
  /// LCM IPC socket name
  std::string jbpf_lcm_ipc_name = JBPF_DEFAULT_LCM_SOCKET;
  /// jbpf ipc memory size
  size_t jbpf_ipc_mem_size = JBPF_HUGEPAGE_SIZE_1GB;
  /// jbpf ipc memory_name
  std::string jbpf_ipc_mem_name = "srsran_ipc_app";
  /// jbpf standalone output IP
  std::string jbpf_standalone_io_out_ip = "127.0.0.1";
  /// jbpf standalone destination UDP port to stream the data
  unsigned jbpf_standalone_io_out_port = 20788;
  /// jbpf standalone UDP Port to receive control inputs
  unsigned jbpf_standalone_io_in_port = 1924;
  /// Standalone IO thread CPU id
  unsigned jbpf_standalone_io_cpu = 0;
  /// Standalone IO thread CPU policy
  unsigned jbpf_standalone_io_policy = 0;
  /// Standalone IO thread CPU priority
  unsigned jbpf_standalone_io_priority = 0;
  /// Requested IO memory size
  unsigned jbpf_io_mem_size_mb = 1024;
  /// Agent thread CPU id
  unsigned jbpf_agent_cpu = 0;
  /// Agent thread CPU policy
  unsigned jbpf_agent_policy = 0;
  /// Agent thread CPU priority
  unsigned jbpf_agent_priority = 0;
  /// Maintenance thread CPU id
  unsigned jbpf_maint_cpu = 0;
  /// Maintenance thread CPU policy
  unsigned jbpf_maint_policy = 0;
  /// Maintenance thread CPU priority
  unsigned jbpf_maint_priority = 0;
};
#endif

/// Monolithic gnb application configuration.
struct gnb_appconfig {
  /// Default constructor to update the log filename.
  gnb_appconfig() { log_cfg.filename = "/tmp/gnb.log"; }
  /// Loggers configuration.
  logger_appconfig log_cfg;
  /// Tracers configuration.
  tracer_appconfig trace_cfg;
  /// Metrics configuration.
  metrics_appconfig metrics_cfg;
  /// gNodeB identifier.
  gnb_id_t gnb_id = {411, 22};
  /// Node name.
  std::string ran_node_name = "gnb01";
  /// Buffer pool configuration.
  app_services::buffer_pool_appconfig buffer_pool_config;
  /// Expert configuration.
  expert_execution_appconfig expert_execution_cfg;
  /// HAL configuration.
  std::optional<hal_appconfig> hal_config;
  /// Remote control configuration.
  remote_control_appconfig remote_control_config;
  /// Dry run mode enabled flag.
  bool enable_dryrun = false;
#ifdef JBPF_ENABLED
  /// jbpf configuration.
  jbpf_appconfig jbpf_cfg;
#endif
};

} // namespace ocudu
