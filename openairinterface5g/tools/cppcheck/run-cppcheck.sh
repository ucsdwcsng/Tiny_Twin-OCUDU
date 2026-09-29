#!/bin/bash
# SPDX-License-Identifier: MIT

cppcheck --enable=warning --force \
    --inline-suppr \
    --quiet \
    -i openair1/PHY/CODING/nrLDPC_decoder/nrLDPC_decoder.c \
    --suppressions-list=tools/cppcheck/suppressions.list \
    -I common/utils \
    -I openair3/NAS/COMMON/UTIL \
    -j`nproc` .
