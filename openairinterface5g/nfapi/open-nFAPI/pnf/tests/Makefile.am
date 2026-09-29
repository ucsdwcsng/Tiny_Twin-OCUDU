#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

#nfapi unit test
AUTOMAKE_OPTIONS=subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/nfapi/inc -I$(top_srcdir)/nfapi/public_inc  -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/pnf/public_inc $(CFLAGS_CUNIT) -Wall -Werror

check_PROGRAMS= test_pnf

test_pnf_SOURCES = pnf_cunit_main.c ../../common/src/debug.c   
test_pnf_LDADD=$(top_builddir)/pnf/libnfapi_pnf.a $(top_builddir)/nfapi/libnfapi.a -L$(libdir) -lpthread -lrt -lsctp -lz -lcunit

LOG_DRIVER = $(top_srcdir)/tap-driver.sh
TESTS=$(check_PROGRAMS)
EXTRA_DIST = $(TESTS)
