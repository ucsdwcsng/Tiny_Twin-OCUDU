#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

AUTOMAKE_OPTIONS=subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/vnf_sim/inc -I$(top_srcdir)/sim_common/inc -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/nfapi/inc -I$(top_srcdir)/nfapi/public_inc -I$(top_srcdir)/vnf/inc -I$(top_srcdir)/vnf/public_inc $(XML_CFLAGS) -Wall -Werror -g
AM_CXXFLAGS = -I$(top_srcidr)/vnf_sim/inc -I$(top_srcdir)/sim_common/inc -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/nfapi/inc -I$(top_srcdir)/nfapi/public_inc -I$(top_srcdir)/vnf/inc -I$(top_srcdir)/vnf/public_inc $(XML_CFLAGS) -std=c++11 $(BOOST_CPPFLAGS) -g
bin_PROGRAMS = vnfsim
vnfsim_SOURCES = src/main.cpp src/mac.cpp
LDADD= $(top_builddir)/vnf/libnfapi_vnf.a $(top_builddir)/common/libnfapi_common.a $(top_builddir)/nfapi/libnfapi.a $(top_builddir)/sim_common/libnfapi_sim_common.a -L$(libdir) -lpthread -lrt -lsctp -lz
