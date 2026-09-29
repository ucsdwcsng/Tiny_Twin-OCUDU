/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "ran_func_rc_subs.h"
#include "common/utils/assertions.h"
#include "common/utils/alg/find.h"

#include <assert.h>
#include <pthread.h>


static pthread_mutex_t rc_mutex = PTHREAD_MUTEX_INITIALIZER;

static bool eq_int(const void* value, const void* it)
{
  const uint32_t ric_req_id = *(uint32_t *)value;
  const ran_param_data_t *dit = (const ran_param_data_t *)it;
  return ric_req_id == dit->ric_req_id;
}

void init_rc_subs_data(rc_subs_data_t *rc_subs_data)
{
  pthread_mutex_lock(&rc_mutex);
  // Initialize sequence array
  seq_arr_init(&rc_subs_data->rs1_param3, sizeof(ran_param_data_t));
  seq_arr_init(&rc_subs_data->rs1_param4, sizeof(ran_param_data_t));
  seq_arr_init(&rc_subs_data->rs4_param202, sizeof(ran_param_data_t));
  pthread_mutex_unlock(&rc_mutex);
}

void insert_rc_subs_data(seq_arr_t *seq_arr, ran_param_data_t *data)
{
  pthread_mutex_lock(&rc_mutex);
  // Insert (RIC request ID + Event Trigger Definition) in specific RAN Parameter ID sequence
  seq_arr_push_back(seq_arr, data, sizeof(*data));
  pthread_mutex_unlock(&rc_mutex);
}

void remove_rc_subs_data(rc_subs_data_t *rc_subs_data, uint32_t ric_req_id)
{
  pthread_mutex_lock(&rc_mutex);
  /* find the sequence element that matches RIC request ID */
  elm_arr_t elm_rs1_param3 = find_if(&rc_subs_data->rs1_param3, (void *)&ric_req_id, eq_int);
  ran_param_data_t *data_rs1_param3 = elm_rs1_param3.it;
  if (data_rs1_param3 != NULL) {
    free_e2sm_rc_event_trigger(&data_rs1_param3->ev_tr);
    seq_arr_erase(&rc_subs_data->rs1_param3, elm_rs1_param3.it);
  }

  /* find the sequence element that matches RIC request ID */
  elm_arr_t elm_rs1_param4 = find_if(&rc_subs_data->rs1_param4, (void *)&ric_req_id, eq_int);
  ran_param_data_t *data_rs1_param4 = elm_rs1_param4.it;
  if (data_rs1_param4 != NULL) {
    free_e2sm_rc_event_trigger(&data_rs1_param4->ev_tr);
    seq_arr_erase(&rc_subs_data->rs1_param4, elm_rs1_param4.it);
  }

  /* find the sequence element that matches RIC request ID */
  elm_arr_t elm_rs4_param202 = find_if(&rc_subs_data->rs4_param202, (void *)&ric_req_id, eq_int);
  ran_param_data_t *data_rs4_param202 = elm_rs4_param202.it;
  if (data_rs4_param202 != NULL) {
    free_e2sm_rc_event_trigger(&data_rs4_param202->ev_tr);
    seq_arr_erase(&rc_subs_data->rs4_param202, elm_rs4_param202.it);
  }
  pthread_mutex_unlock(&rc_mutex);
}
