/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef GET_MPLANE_INFO_XML_H
#define GET_MPLANE_INFO_XML_H

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief retrieves the node value that matches filter.
 *
 * @param[in] buffer Buffer in xml format.
 * @param[in] filter The node name (matching criteria).
 * @return Expected one string that matches the node name.
 */
char *get_ru_xml_node(const char *buffer, const char *filter);

/**
 * @brief retrieves the node values that matches filter.
 *
 * @param[in] buffer Buffer in xml format.
 * @param[in] filter The node name (matching criteria).
 * @param[out] match_list Resulting list containing node values.
 * @param[out] count The number of nodes that match criteria.
 * @return Void.
 */
void get_ru_xml_list(const char *buffer, const char *filter, char ***match_list, size_t *count);

#endif /* GET_MPLANE_INFO_XML_H */
