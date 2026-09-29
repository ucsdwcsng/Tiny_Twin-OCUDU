#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

set -eu

# sets final exit code and thus jenkins pass or fail
STATUS=$(jq -r '.status' results.json)
echo "job status: ${STATUS}"
[ "${STATUS}" = "successful" ]
