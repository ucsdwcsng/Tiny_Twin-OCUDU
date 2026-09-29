#!/bin/bash
# SPDX-License-Identifier: MIT

set -euo pipefail

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <path-to-validate_nrppa.py> <path-to-InputData.json>" >&2
    exit 1
fi

# Processing directory: where the python script and JSON payload live
VALIDATE_SCRIPT="$1"
SCRIPT_DIR="$(dirname "${VALIDATE_SCRIPT}")"
INPUT_JSON="$2"

LMF_URL="http://192.168.70.141:8080/nlmf-loc/v1/determine-location"

GNB_CONTAINER="rfsim5g-oai-gnb"
LMF_CONTAINER="oai-lmf"

GNB_LOG_FILE="${SCRIPT_DIR}/gnb_pos_logs.txt"
LMF_LOG_FILE="${SCRIPT_DIR}/oai_lmf_logs.txt"

if [[ ! -f "${VALIDATE_SCRIPT}" ]]; then
    echo "ERROR: Input payload not found at ${VALIDATE_SCRIPT}" >&2
    exit 1
fi

if [[ ! -f "${INPUT_JSON}" ]]; then
    echo "ERROR: validate_nrppa.py not found at ${INPUT_JSON}" >&2
    exit 1
fi

command -v curl  >/dev/null 2>&1 || { echo "ERROR: curl is required."  >&2; exit 1; }
command -v docker >/dev/null 2>&1 || { echo "ERROR: docker is required." >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "ERROR: python3 is required." >&2; exit 1; }

echo "=== [1/3] Sending location determination request to LMF ==="
HTTP_STATUS=$(curl --http2-prior-knowledge \
    -sS \
    -o "${SCRIPT_DIR}/lmf_response.json" \
    -w "%{http_code}" \
    -H "Content-Type: application/json" \
    -d "@${INPUT_JSON}" \
    --connect-timeout 30 \
    -X POST "${LMF_URL}")

# Give the containers a moment to finish logging the exchange
sleep 2

echo "=== [2/3] Collecting docker logs ==="
docker logs "${GNB_CONTAINER}" > "${GNB_LOG_FILE}" 2>&1
echo "Saved gNB logs to ${GNB_LOG_FILE}"

docker logs "${LMF_CONTAINER}" > "${LMF_LOG_FILE}" 2>&1
echo "Saved LMF logs to ${LMF_LOG_FILE}"

echo "=== [3/3] Running K1 validation ==="
set +e
python3 "${VALIDATE_SCRIPT}" --lmf-log "${LMF_LOG_FILE}" --gnb-log "${GNB_LOG_FILE}"
VALIDATION_EXIT_CODE=$?
set -e

exit "${VALIDATION_EXIT_CODE}"
