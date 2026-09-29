// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#include "btf_json.h"

#include "btf_c_type.h"

#include <functional>
#include <set>

namespace libbtf {
std::string pretty_print_json(const std::string &input) {
  // Walk over the input string, inserting newlines and indentation.
  std::string output;
  int indent = 0;
  bool in_string = false;
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    if (c == '"') {
      in_string = !in_string;
    }
    if (in_string) {
      output += c;
      continue;
    }
    switch (c) {
    case '{':
    case '[':
      output += c;
      if (i + 1 < input.size() && input[i + 1] != '}' && input[i + 1] != ']') {
        output += '\n';
        indent += 2;
        output += std::string(indent, ' ');
      } else {
        output += input[++i];
      }
      break;
    case '}':
    case ']':
      output += '\n';
      indent -= 2;
      output += std::string(indent, ' ');
      output += c;
      break;
    case ',':
      output += c;
      output += '\n';
      output += std::string(indent, ' ');
      break;
    case ':':
      output += c;
      output += ' ';
      break;
    default:
      output += c;
      break;
    }
  }
  return output;
}

template <typename T>
static void _print_json_value(bool &first, const std::string &name, T value,
                              std::ostream &out) {
  // If T is a string type, print it as a string, then quote the value.
  if constexpr (std::is_same_v<T, std::string> ||
                std::is_same_v<T, const char *> || std::is_same_v<T, char *>) {
    out << (first ? "" : ",") << "\"" << name << "\":\"" << value << "\"";
    first = false;
  }
  // If T is a bool, print it as a string, then quote the value.
  else if constexpr (std::is_same_v<T, bool>) {
    out << (first ? "" : ",") << "\"" << name
        << "\":" << (value ? "true" : "false");
    first = false;
  }
  // If T is a std::optional<std::string>, then only print if it's present
  else if constexpr (std::is_same_v<T, std::optional<std::string>>) {
    if (value.has_value()) {
      out << (first ? "" : ",") << "\"" << name << "\":\"" << value.value()
          << "\"";
      first = false;
    }
  } else {
    out << (first ? "" : ",") << "\"" << name << "\":" << std::to_string(value);
    first = false;
  }
}

void print_array_start(const std::string &name, std::ostream &out) {
  out << "\"" << name << "\":[";
}

void print_array_end(std::ostream &out) { out << "]"; }

#define PRINT_JSON_FIXED(name, value)                                          \
  _print_json_value(first, name, value, out);

#define PRINT_JSON_VALUE(object, value)                                        \
  _print_json_value(first, #value, object.value, out)

#define PRINT_JSON_TYPE(object, value)                                         \
  if (!first) {                                                                \
    out << ",";                                                                \
  } else {                                                                     \
    first = false;                                                             \
  };                                                                           \
  out << "\"" << #value << "\":";                                              \
  print_btf_kind(object.value, id_to_kind.at(object.value));

#define PRINT_JSON_ARRAY_START(object, value)                                  \
  if (!first) {                                                                \
    out << ",";                                                                \
  } else {                                                                     \
    first = false;                                                             \
  }                                                                            \
  print_array_start(#value, out);                                              \
  {                                                                            \
    bool first = true;

#define PRINT_JSON_ARRAY_END()                                                 \
  print_array_end(out);                                                        \
  }

#define PRINT_JSON_OBJECT_START()                                              \
  if (!first) {                                                                \
    out << ",";                                                                \
  } else {                                                                     \
    first = false;                                                             \
  };                                                                           \
  {                                                                            \
    bool first = true;                                                         \
    out << "{";

#define PRINT_JSON_OBJECT_END()                                                \
  out << "}";                                                                  \
  }

#define PRINT_JSON_VALUE_IF_PRESENT(object, value)                             \
  if constexpr (btf_kind_traits<decltype(object)>::has_##value) {              \
    PRINT_JSON_VALUE(object, value);                                           \
  }

#define PRINT_JSON_TYPE_IF_PRESENT(object, value)                              \
  if constexpr (btf_kind_traits<decltype(object)>::has_##value) {              \
    PRINT_JSON_TYPE(object, value);                                            \
  }

// Suppress C4456 on when using MSVC:
// declaration of 'first' hides previous local declaration
#pragma warning(push)
#pragma warning(disable : 4456)
#pragma warning(disable : 4458)

void btf_type_to_json(const std::map<btf_type_id, btf_kind> &id_to_kind,
                      std::ostream &out,
                      std::optional<std::function<bool(btf_type_id)>> filter) {
  std::function<void(btf_type_id, const btf_kind &)> print_btf_kind =
      [&](btf_type_id id, const btf_kind &kind) {
        bool first = true;
        PRINT_JSON_OBJECT_START();
        PRINT_JSON_FIXED("id", id);
        PRINT_JSON_FIXED("kind_type",
                         BTF_KIND_INDEX_TO_STRING(
                             static_cast<btf_kind_index>(kind.index())));

        std::visit(
            [&](auto &kind) {
              // Print JSON values.
              PRINT_JSON_VALUE_IF_PRESENT(kind, name);
              if constexpr (btf_kind_traits<decltype(kind)>::has_linkage) {
                PRINT_JSON_FIXED("linkage",
                                 BTF_KIND_LINKAGE_TO_STRING(kind.linkage));
              }
              PRINT_JSON_VALUE_IF_PRESENT(kind, count_of_elements);
              PRINT_JSON_VALUE_IF_PRESENT(kind, size_in_bytes);
              PRINT_JSON_VALUE_IF_PRESENT(kind, size);
              PRINT_JSON_VALUE_IF_PRESENT(kind, is_struct);
              PRINT_JSON_VALUE_IF_PRESENT(kind, offset_from_start_in_bits);
              PRINT_JSON_VALUE_IF_PRESENT(kind, field_width_in_bits);
              PRINT_JSON_VALUE_IF_PRESENT(kind, is_signed);
              PRINT_JSON_VALUE_IF_PRESENT(kind, is_char);
              PRINT_JSON_VALUE_IF_PRESENT(kind, is_bool);

              // Print JSON arrays.
              if constexpr (btf_kind_traits<decltype(kind)>::has_members) {
                PRINT_JSON_ARRAY_START(kind, members);
                for (auto &member : kind.members) {
                  PRINT_JSON_OBJECT_START();
                  PRINT_JSON_VALUE_IF_PRESENT(member, name);
                  PRINT_JSON_VALUE_IF_PRESENT(member, value);
                  PRINT_JSON_VALUE_IF_PRESENT(member,
                                              offset_from_start_in_bits);
                  PRINT_JSON_VALUE_IF_PRESENT(member, offset);
                  PRINT_JSON_VALUE_IF_PRESENT(member, size);
                  PRINT_JSON_TYPE_IF_PRESENT(member, type);
                  PRINT_JSON_OBJECT_END();
                }
                PRINT_JSON_ARRAY_END();
              }

              if constexpr (btf_kind_traits<decltype(kind)>::has_parameters) {
                PRINT_JSON_ARRAY_START(kind, parameters);
                for (auto &parameter : kind.parameters) {
                  PRINT_JSON_OBJECT_START();
                  PRINT_JSON_VALUE_IF_PRESENT(parameter, name);
                  PRINT_JSON_TYPE_IF_PRESENT(parameter, type);
                  PRINT_JSON_OBJECT_END();
                }
                PRINT_JSON_ARRAY_END();
              }

              // Print JSON child object.
              PRINT_JSON_TYPE_IF_PRESENT(kind, type);
              PRINT_JSON_TYPE_IF_PRESENT(kind, element_type);
              PRINT_JSON_TYPE_IF_PRESENT(kind, index_type);
              PRINT_JSON_TYPE_IF_PRESENT(kind, return_type);
            },
            kind);
        PRINT_JSON_OBJECT_END();
      };

  // Determine the list of types that are not referenced by other types. These
  // are the root types.
  std::set<btf_type_id> root_types;

  // Add all types as root types.
  for (auto &[id, kind] : id_to_kind) {
    root_types.insert(id);
  }

  // Erase the VOID type.
  root_types.erase(0);

  // Remove all types that are referenced by other types.
  for (auto &[id, kind] : id_to_kind) {
    if (filter.has_value()) {
      if (!(*filter)(id)) {
        root_types.erase(id);
      }
      continue;
    }

    std::visit(
        [&](const auto &kind) {
          if constexpr (btf_kind_traits<decltype(kind)>::has_type) {
            root_types.erase(kind.type);
          }
          if constexpr (btf_kind_traits<decltype(kind)>::has_element_type) {
            root_types.erase(kind.element_type);
          }
          if constexpr (btf_kind_traits<decltype(kind)>::has_index_type) {
            root_types.erase(kind.index_type);
          }
          if constexpr (btf_kind_traits<decltype(kind)>::has_members) {
            for (auto &member : kind.members) {
              if constexpr (btf_kind_traits<decltype(member)>::has_type) {
                root_types.erase(member.type);
              }
            }
          }
          if constexpr (btf_kind_traits<decltype(kind)>::has_parameters) {
            for (auto &parameter : kind.parameters) {
              root_types.erase(parameter.type);
            }
          }
          if constexpr (btf_kind_traits<decltype(kind)>::has_return_type) {
            root_types.erase(kind.return_type);
          }
        },
        kind);
  }
  bool first = true;
  PRINT_JSON_OBJECT_START();
  PRINT_JSON_ARRAY_START("", btf_kinds);
  for (const auto &[id, kind] : id_to_kind) {
    // Skip non-root types.
    if (root_types.find(id) == root_types.end()) {
      continue;
    }

    out << (first ? "" : ",");
    first = false;
    print_btf_kind(id, kind);
  }
  PRINT_JSON_ARRAY_END();
  PRINT_JSON_OBJECT_END();
}

#pragma warning(pop)

} // namespace libbtf