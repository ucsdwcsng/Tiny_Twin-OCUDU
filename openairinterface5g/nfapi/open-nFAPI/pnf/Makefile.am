#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

AUTOMAKE_OPTIONS=subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/pnf/inc -I$(top_srcdir)/pnf/public_inc -I$(top_srcdir)/nfapi/public_inc -I$(top_srcdir)/common/public_inc -g -Wall -Werror


noinst_LIBRARIES = libnfapi_pnf.a

libnfapi_pnf_a_SOURCES = src/pnf.c\
						 src/pnf_interface.c\
						 src/pnf_p7.c\
						 src/pnf_p7_interface.c

LDADD = -lsctp



