#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

#vnf unit test
#AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/nfapi/inc -I$(top_srcdir)/nfapi/public_inc  -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/vnf/public_inc $(CFLAGS_CUNIT) -Wall -Werror

check_PROGRAMS= test_vnf

test_vnf_SOURCES = vnf_cunit_main.c ../..//common/src/debug.c   
test_vnf_LDADD=$(top_builddir)/vnf/libnfapi_vnf.a $(top_builddir)/nfapi/libnfapi.a  -L$(libdir) -lpthread -lrt -lsctp -lz -lcunit

LOG_DRIVER = $(top_srcdir)/tap-driver.sh
TESTS=test_vnf
EXTRA_DIST = $(TESTS)

