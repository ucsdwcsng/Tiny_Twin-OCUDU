/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "get-xml.h"

#include <libxml/parser.h>
#include <string.h>
#include <stdlib.h>

static xmlChar *find_ru_xml_node(xmlNode *node, const char *filter)
{
  for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type != XML_ELEMENT_NODE)
      continue;

    if (strcmp((const char *)cur_node->name, filter) == 0) {
      xmlNode *target_node = cur_node;

      for(xmlNode *cur_node2 = cur_node->children; cur_node2; cur_node2 = cur_node2->next) {
        if (cur_node2->type == XML_ELEMENT_NODE && strcmp((const char *)cur_node2->name, "name") == 0) {
          if (strcmp((const char *)cur_node2->next->next->name, "type") == 0) {
            xmlChar *content = xmlNodeGetContent(cur_node2->next->next);
            if (strcmp((const char *)content, "ianaift:ethernetCsmacd") != 0) {
              xmlFree(content);
              xmlChar *answer = find_ru_xml_node(cur_node->next->next, filter);
              if (answer != NULL) {
                return answer;
              }
            }
            xmlFree(content);
          }
          target_node = cur_node2;
          break;
        }
      }
      return xmlNodeGetContent(target_node);
    }
    xmlChar *answer = find_ru_xml_node(cur_node->children, filter);
    if (answer != NULL) {
      return answer;
    }
  }
  return NULL;
}

char *get_ru_xml_node(const char *buffer, const char *filter)
{
  // Initialize the xml file
  size_t len = strlen(buffer) + 1;
  xmlDoc *doc = xmlReadMemory(buffer, len, NULL, NULL, 0);
  xmlNode *root_element = xmlDocGetRootElement(doc);

  xmlChar *content = find_ru_xml_node(root_element->children, filter);

  char *value = strdup((char *)content);
  xmlFree(content);
  xmlFreeDoc(doc);

  return value;
}

static void find_ru_xml_list(xmlNode *node, const char *filter, char ***match_list, size_t *count)
{
  for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type != XML_ELEMENT_NODE)
      continue;
    if (strcmp((const char *)cur_node->name, filter) == 0) {
      xmlNode *name_node = NULL;

      for (xmlNode *cur_node2 = cur_node->children; cur_node2; cur_node2 = cur_node2->next) {
        const char *name_node_str = (const char *)cur_node2->name;
        if (cur_node2->type == XML_ELEMENT_NODE && (strcmp(name_node_str, "name") == 0 || strcmp(name_node_str, "measurement-object") == 0)) {
          name_node = cur_node2;
          break;
        }
      }
      xmlChar *content = xmlNodeGetContent(name_node ? name_node : cur_node);
      if (content) {
        *match_list = realloc(*match_list, (*count + 1) * sizeof(char *));
        (*match_list)[*count] = strdup((char *)content);
        (*count)++;
	xmlFree(content);
      }
    }
    find_ru_xml_list(cur_node->children, filter, match_list, count);
  }
}

void get_ru_xml_list(const char *buffer, const char *filter, char ***match_list, size_t *count)
{
  // Initialize the xml file
  size_t len = strlen(buffer) + 1;
  xmlDoc *doc = xmlReadMemory(buffer, len, NULL, NULL, 0);
  xmlNode *root_element = xmlDocGetRootElement(doc);

  find_ru_xml_list(root_element->children, filter, match_list, count);

  xmlFreeDoc(doc);
}
