#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

set -eu

AWX_API_URL=https://10.100.1.253
AWX_JOB_EVENT_QUERY=job_events/?search=results_url

COL_USER=$1
COL_PASS=$2

AWX_API_JOB_PATH=$(jq -r '.url' launch.json)

# get result url and download test results
curl -s -f -k -u ${COL_USER}:${COL_PASS} -X GET ${AWX_API_URL}${AWX_API_JOB_PATH}${AWX_JOB_EVENT_QUERY} > result.json

set -x
RESULT=$(jq -r '.results[0].event_data.res.ansible_facts.results_url' result.json)
wget -q -O - ${RESULT} > results.tar.xz
