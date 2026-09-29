// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace libbtf {
typedef uint32_t btf_type_id;

enum btf_kind_index {
  BTF_KIND_VOID,
  BTF_KIND_INT,
  BTF_KIND_PTR,
  BTF_KIND_ARRAY,
  BTF_KIND_STRUCT,
  BTF_KIND_UNION,
  BTF_KIND_ENUM,
  BTF_KIND_FWD,
  BTF_KIND_TYPEDEF,
  BTF_KIND_VOLATILE,
  BTF_KIND_CONST,
  BTF_KIND_RESTRICT,
  BTF_KIND_FUNCTION,
  BTF_KIND_FUNCTION_PROTOTYPE,
  BTF_KIND_VAR,
  BTF_KIND_DATA_SECTION,
  BTF_KIND_FLOAT,
  BTF_KIND_DECL_TAG,
  BTF_KIND_TYPE_TAG,
  BTF_KIND_ENUM64,
};

enum btf_kind_linkage {
  BTF_LINKAGE_STATIC,
  BTF_LINKAGE_GLOBAL,
  BTF_LINKAGE_EXTERN,
};

#define BTF_ENUM_TO_STRING_HELPER(X)                                           \
  case X:                                                                      \
    return #X;

static inline const char *BTF_KIND_INDEX_TO_STRING(btf_kind_index index) {
  switch (index) {
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_VOID);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_INT);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_PTR);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_ARRAY);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_STRUCT);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_UNION);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_ENUM);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_FWD);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_TYPEDEF);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_VOLATILE);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_CONST);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_RESTRICT);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_FUNCTION);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_FUNCTION_PROTOTYPE);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_VAR);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_DATA_SECTION);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_FLOAT);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_DECL_TAG);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_TYPE_TAG);
    BTF_ENUM_TO_STRING_HELPER(BTF_KIND_ENUM64);
  default:
    return "UNKNOWN";
  }
}

static inline const char *BTF_KIND_LINKAGE_TO_STRING(btf_kind_linkage linkage) {
  switch (linkage) {
    BTF_ENUM_TO_STRING_HELPER(BTF_LINKAGE_STATIC);
    BTF_ENUM_TO_STRING_HELPER(BTF_LINKAGE_GLOBAL);
    BTF_ENUM_TO_STRING_HELPER(BTF_LINKAGE_EXTERN);
  default:
    return "UNKNOWN";
  }
}

struct btf_kind_int {
  std::string name;
  uint32_t size_in_bytes; // The size of the integer in bytes. This value
                          // multiplied by 8 must be >= field_width_in_bits
  uint16_t offset_from_start_in_bits; // The start of the integer relative to
                                      // the start of the member.
  uint8_t field_width_in_bits;        // The size of the integer in bits.
  bool is_signed;
  bool is_char;
  bool is_bool;
};

struct btf_kind_ptr {
  btf_type_id type;
};

struct btf_kind_array {
  btf_type_id element_type;
  btf_type_id index_type;
  uint32_t count_of_elements;
};

struct btf_kind_struct_member {
  std::optional<std::string> name;
  btf_type_id type;
  uint32_t offset_from_start_in_bits;
};

using btf_kind_union_member = btf_kind_struct_member;

struct btf_kind_struct {
  std::optional<std::string> name;
  std::vector<btf_kind_struct_member> members;
  uint32_t size_in_bytes;
};

struct btf_kind_union {
  std::optional<std::string> name;
  std::vector<btf_kind_union_member> members;
  uint32_t size_in_bytes;
};

struct btf_kind_enum_member {
  std::string name;
  uint32_t value;
};

struct btf_kind_enum {
  std::optional<std::string> name;
  bool is_signed;
  std::vector<btf_kind_enum_member> members;
  uint32_t size_in_bytes;
};

struct btf_kind_fwd {
  std::string name;
  bool is_struct;
};

struct btf_kind_typedef {
  std::string name;
  btf_type_id type;
};

struct btf_kind_volatile {
  btf_type_id type;
};

struct btf_kind_const {
  btf_type_id type;
};

struct btf_kind_restrict {
  btf_type_id type;
};

struct btf_kind_function {
  std::string name;
  btf_kind_linkage linkage;
  btf_type_id type;
};

struct btf_kind_function_parameter {
  std::string name;
  btf_type_id type;
};

struct btf_kind_function_prototype {
  std::vector<btf_kind_function_parameter> parameters;
  btf_type_id return_type;
};

struct btf_kind_var {
  std::string name;
  btf_type_id type;
  btf_kind_linkage linkage;
};

struct btf_kind_data_member {
  btf_type_id type;
  uint32_t offset;
  uint32_t size;
};

struct btf_kind_data_section {
  std::string name;
  std::vector<btf_kind_data_member> members;
  uint32_t size;
};

struct btf_kind_float {
  std::string name;
  uint32_t size_in_bytes;
};

struct btf_kind_decl_tag {
  std::string name;
  btf_type_id type;
  uint32_t component_index;
};

struct btf_kind_type_tag {
  std::string name;
  btf_type_id type;
};

struct btf_kind_enum64_member {
  std::string name;
  uint64_t value;
};

struct btf_kind_enum64 {
  std::optional<std::string> name;
  bool is_signed;
  std::vector<btf_kind_enum64_member> members;
  uint32_t size_in_bytes;
};

struct btf_kind_void {};

template <typename T> struct btf_kind_traits {
  constexpr static bool has_optional_name = requires(const T &value) {
    value.name.has_value();
  };
  constexpr static bool has_name = requires(const T &value) { value.name; };
  constexpr static bool has_members = requires(const T &value) {
    value.members;
  };
  constexpr static bool has_parameters = requires(const T &value) {
    value.parameters;
  };
  constexpr static bool has_return_type = requires(const T &value) {
    value.return_type != 0;
  };
  constexpr static bool has_type = requires(const T &value) {
    value.type != 0;
  };
  constexpr static bool has_size_in_bytes = requires(const T &value) {
    value.size_in_bytes;
  };
  constexpr static bool has_linkage = requires(const T &value) {
    value.linkage;
  };
  constexpr static bool has_count_of_elements = requires(const T &value) {
    value.count_of_elements;
  };
  constexpr static bool has_element_type = requires(const T &value) {
    value.element_type;
  };
  constexpr static bool has_index_type = requires(const T &value) {
    value.index_type;
  };
  constexpr static bool has_offset_from_start_in_bits =
      requires(const T &value) {
    value.offset_from_start_in_bits;
  };
  constexpr static bool has_offset = requires(const T &value) { value.offset; };
  constexpr static bool has_size = requires(const T &value) { value.size; };
  constexpr static bool has_value = requires(const T &value) { value.value; };
  constexpr static bool has_is_struct = requires(const T &value) {
    value.is_struct;
  };
  constexpr static bool has_field_width_in_bits = requires(const T &value) {
    value.field_width_in_bits;
  };
  constexpr static bool has_is_signed = requires(const T &value) {
    value.is_signed;
  };
  constexpr static bool has_is_char = requires(const T &value) {
    value.is_char;
  };
  constexpr static bool has_is_bool = requires(const T &value) {
    value.is_bool;
  };
};

// Note: The order of the variant types must match the order in the enum above.
using btf_kind = std::variant<
    btf_kind_void, btf_kind_int, btf_kind_ptr, btf_kind_array, btf_kind_struct,
    btf_kind_union, btf_kind_enum, btf_kind_fwd, btf_kind_typedef,
    btf_kind_volatile, btf_kind_const, btf_kind_restrict, btf_kind_function,
    btf_kind_function_prototype, btf_kind_var, btf_kind_data_section,
    btf_kind_float, btf_kind_decl_tag, btf_kind_type_tag, btf_kind_enum64>;
} // namespace libbtf