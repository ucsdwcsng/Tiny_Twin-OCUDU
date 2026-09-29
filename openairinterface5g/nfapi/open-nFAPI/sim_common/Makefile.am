#
# SPDX-License-Identifier: Apache-2.0
# 
# Copyright 2017 Cisco Systems, Inc.
#

AUTOMAKE_OPTIONS=subdir-objects

AM_CFLAGS = -I$(top_srcdir)/sim_common/public_inc -I$(top_srcdir)/common/public_inc -I$(top_srcdir)/nfapi/public_inc -g -Wall -Werror

AM_CXXFLAGS = -I$(top_srcdir)/sim_common/public_inc -I$(top_srcdir)/common/public_inc -std=c++11 -I$(top_srcdir)/nfapi/public_inc $(BOOST_CPPFLAGS) -g -Wall -Werror
noinst_LIBRARIES =libnfapi_sim_common.a
#libnfapi_sim_common_a_SOURCES = src/xml_property_tree.cpp src/pool.cpp
libnfapi_sim_common_a_SOURCES = src/pool.cpp

libnfapi_sim_common_a_CFLAGS =$(AM_CFLAGS)
libnfapi_sim_common_a_CXXFLAGS =$(AM_CXXFLAGS)

lib_LTLIBRARIES =libnfapi_sim_common.la
#libnfapi_sim_common_la_SOURCES = src/xml_property_tree.cpp
libnfapi_sim_common_la_SOURCES = src/pool.cpp
