#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

#vnf unit test
AM_CPPFLAGS = -I../nfapi/inc -I../nfapi/public_inc  -I../common/public_inc -I../vnf/public_inc $(CFLAGS_CUNIT) -Wall -Werror

export LD_LIBRARY_PATH=/opt/gcc-4.7.3/lib
check_PROGRAMS= nfapi_integration_tests
nfapi_integration_tests_SOURCES = main.cpp ../common/src/debug.c   
nfapi_integration_tests_LDADD= -L$(libdir) -lpthread -lrt  -lcunit



LOG_DRIVER = $(top_srcdir)/tap-driver.sh
TESTS=nfapi_integration_tests
EXTRA_DIST=$(TESTS)
