#!/bin/bash
# SPDX-License-Identifier: MIT

export PS4='[\D{%Y-%m-%d %H:%M:%S}] '

function die() { echo $@; exit 1; }
[ $# -eq 3 ] || die "usage: $0 <path-to-dir> <namespace> <image-tag>"

OC_NS=${2}
IMAGE_TAG=${3}
OC_DIR=${1}
OC_RELEASE=$(basename "${1}")

cat /opt/oc-password | oc login -u oaicicd --server https://api.oai.cs.eurecom.fr:6443 --insecure-skip-tls-verify > /dev/null
oc project ${OC_NS} > /dev/null
set -x
helm install --wait --timeout 120s ${OC_RELEASE} --set nfimage.version=${IMAGE_TAG} --hide-notes ${OC_DIR}/.
status=$?
set +x
oc logout > /dev/null

[ $status -eq 0 ] || die "OC chart deployment failed"
