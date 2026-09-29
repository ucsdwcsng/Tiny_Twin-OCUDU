#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

set -eu

AWX_API_URL=https://10.100.1.253
AWX_JOB_ID=16
AWX_API_LAUNCH_PATH=/api/v2/job_templates/${AWX_JOB_ID}/launch/

COL_USER=$1
COL_PASS=$2
JENKINS_JOB_ID=$3
GIT_REPOSITORY=$4
GIT_BRANCH=$5
COLOSSEUM_RF_SCENARIO=$6
JENKINS_JOB_URL=$7

# assemble data to send
CURL_DATA=$(cat <<-END | jq -c .
{
    "extra_vars":
    {
        "oai_repo": "${GIT_REPOSITORY}",
        "oai_branch": "${GIT_BRANCH}",
        "colosseum_rf_scenario": "${COLOSSEUM_RF_SCENARIO}",
        "jenkins_job_id": "${JENKINS_JOB_ID}",
        "jenkins_job_url": "${JENKINS_JOB_URL}"
    }
}
END
)

# launch job
curl -s -f -k -u ${COL_USER}:${COL_PASS} -X POST -H "Content-Type: application/json" -d ${CURL_DATA} ${AWX_API_URL}${AWX_API_LAUNCH_PATH} > launch.json
