#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

AUTOMAKE_OPTIONS=subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/nfapi/inc -I$(top_srcdir)/nfapi/public_inc  -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/pnf/public_inc  $(BOOST_CPPFLAGS) -Wall -Werror -g

noinst_LIBRARIES = libnfapi.a

libnfapi_a_SOURCES = src/nfapi.c  src/nfapi_p5.c  src/nfapi_p7.c src/nfapi_p4.c
libnfapi_a_CFLAGS =$(AM_CFLAGS)

lib_LTLIBRARIES = libnfapi.la
libnfapi_la_SOURCES =  src/nfapi.c  src/nfapi_p5.c  src/nfapi_p7.c src/nfapi_p4.c


LDADD= ../pnf/src/libnfapi_pnf.a ../vnf/src/libnfapi_vnf.a ../common/bin/libnfapi_common.a 

