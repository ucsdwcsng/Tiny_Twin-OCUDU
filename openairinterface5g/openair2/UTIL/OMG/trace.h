/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief The trace-based mobility model for OMG/OAI (mobility is statically imported from a file)
 */

#ifndef TRACE_H_
#define TRACE_H_
//#include "defs.h"
#include "omg.h"
#include "trace_hashtable.h"
#include "mobility_parser.h"

int start_trace_generator (omg_global_param omg_param_list);

void place_trace_node (node_struct* node, node_data* n);

void move_trace_node (pair_struct* pair, node_data* n_data, double cur_time);

void schedule_trace_node ( pair_struct* pair, node_data* n_data, double cur_time);

void sleep_trace_node ( pair_struct* pair, node_data* n_data, double cur_time);

void update_trace_nodes (double cur_time);

void get_trace_positions_updated (double cur_time);
void clear_list (void);

#endif /* TRACE_H_ */
