#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#


AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/vnf/inc\
			  -I$(top_srcdir)/vnf/public_inc\
			  -I$(top_srcdir)/nfapi/public_inc\
			  -I$(top_srcdir)/common/public_inc\
			  -Wall -Werror -g


noinst_LIBRARIES = libnfapi_vnf.a

libnfapi_vnf_a_SOURCES = src/vnf.c\
						 src/vnf_p7.c\
						 src/vnf_interface.c\
						 src/vnf_p7_interface.c

LDADD = -lsctp  ../common/bin/libnfapi_common.a

libnfapi_vnf_a_CFLAGS =$(AM_CFLAGS)

lib_LTLIBRARIES = libnfapi_vnf.la

libnfapi_vnf_la_SOURCES =  src/vnf.c\
						 src/vnf_p7.c\
						 src/vnf_interface.c\
						 src/vnf_p7_interface.c
