#!/bin/bash
# SPDX-License-Identifier: MIT

function die() {
  echo $@
  exit 1
}

[ $# -eq 3 ] || die "usage: $0 <directory> <repository> <branch>"

set -ex

dir=$1
repo=$2
branch=$3

rm -rf ${dir}
git clone --depth=1 --branch "${branch}" "${repo}" "${dir}"
cd ${dir}
mkdir -p cmake_targets/log
exit 0
