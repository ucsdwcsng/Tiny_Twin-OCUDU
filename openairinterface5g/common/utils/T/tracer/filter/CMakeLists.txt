# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(tracer_filter STATIC filter.c)
target_include_directories(tracer_filter PRIVATE ..)
