#!/bin/sh
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

test_count=17

for i in `seq $test_count`
do
  make all TEST=test$i >/dev/null 2>/dev/null
  if [ $? != 0 ]
  then
    echo TEST $i FAILURE
  fi
done
