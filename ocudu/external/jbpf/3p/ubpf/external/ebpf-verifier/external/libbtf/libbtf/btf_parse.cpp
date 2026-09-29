// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#include "btf_parse.h"

#include "btf_c_type.h"

#include <cstring>
#include <stdexcept>

namespace libbtf {
template <typename T>
static T read_btf(const std::vector<std::byte> &btf, size_t &offset,
                  size_t minimum_offset = 0, size_t maximum_offset = 0) {
  size_t length = 0;
  if (maximum_offset == 0) {
    maximum_offset = btf.size();
  }
  if (offset < minimum_offset || offset > maximum_offset) {
    throw std::runtime_error("Invalid .BTF section - invalid offset");
  }

  if constexpr (std::is_same<T, std::string>::value) {
    length = strnlen(reinterpret_cast<const char *>(btf.data()) + offset,
                     maximum_offset - offset);
    offset += length + 1;
    if (offset > maximum_offset) {
      throw std::runtime_error("Invalid .BTF section - invalid string length");
    }
    return std::string(reinterpret_cast<const char *>(btf.data()) + offset -
                           length - 1,
                       length);
  } else {
    length = sizeof(T);
    offset += length;
    if (offset > maximum_offset) {
      throw std::runtime_error("Invalid .BTF section - invalid type length");
    }
    return *reinterpret_cast<const T *>(btf.data() + offset - length);
  }
}

static void validate_offset(std::vector<std::byte> const &btf, size_t offset) {
  if (offset < 0) {
    throw std::runtime_error("Invalid .BTF section - invalid offset");
  }

  if (offset > btf.size()) {
    throw std::runtime_error("Invalid .BTF section - invalid offset");
  }
}

static void validate_range(std::vector<std::byte> const &btf, size_t start,
                           size_t end) {
  validate_offset(btf, start);
  validate_offset(btf, end);

  if (start > end) {
    throw std::runtime_error("Invalid .BTF section - invalid range");
  }
}

static std::map<size_t, std::string>
_btf_parse_string_table(const std::vector<std::byte> &btf) {
  std::map<size_t, std::string> string_table;

  size_t offset = 0;
  auto btf_header = read_btf<btf_header_t>(btf, offset);
  if (btf_header.magic != BTF_HEADER_MAGIC) {
    throw std::runtime_error("Invalid .BTF section - wrong magic");
  }
  if (btf_header.version != BTF_HEADER_VERSION) {
    throw std::runtime_error("Invalid .BTF section - wrong version");
  }
  if (btf_header.hdr_len < sizeof(btf_header_t)) {
    throw std::runtime_error("Invalid .BTF section - wrong size");
  }
  if (btf_header.hdr_len > btf.size()) {
    throw std::runtime_error("Invalid .BTF section - invalid header length");
  }

  size_t string_table_start = static_cast<size_t>(btf_header.hdr_len) +
                              static_cast<size_t>(btf_header.str_off);
  size_t string_table_end =
      string_table_start + static_cast<size_t>(btf_header.str_len);

  validate_range(btf, string_table_start, string_table_end);

  for (offset = string_table_start; offset < string_table_end;) {
    size_t string_offset = offset - string_table_start;
    std::string value = read_btf<std::string>(btf, offset, string_table_start,
                                              string_table_end);
    if (offset > string_table_end) {
      throw std::runtime_error("Invalid .BTF section - invalid string length");
    }
    string_table.insert({string_offset, value});
  }
  return string_table;
}

static std::string
_btf_find_string(const std::map<size_t, std::string> &string_table,
                 size_t string_offset) {
  auto it = string_table.find(string_offset);
  if (it == string_table.end()) {
    throw std::runtime_error(
        std::string("Invalid .BTF section - invalid string offset"));
  }
  return it->second;
}

void btf_parse_line_information(const std::vector<std::byte> &btf,
                                const std::vector<std::byte> &btf_ext,
                                btf_line_info_visitor visitor) {
  std::map<size_t, std::string> string_table = _btf_parse_string_table(btf);

  size_t btf_ext_offset = 0;
  auto bpf_ext_header = read_btf<btf_ext_header_t>(btf_ext, btf_ext_offset);
  if (bpf_ext_header.hdr_len < sizeof(btf_ext_header_t)) {
    throw std::runtime_error("Invalid .BTF.ext section - wrong size");
  }
  if (bpf_ext_header.magic != BTF_HEADER_MAGIC) {
    throw std::runtime_error("Invalid .BTF.ext section - wrong magic");
  }
  if (bpf_ext_header.version != BTF_HEADER_VERSION) {
    throw std::runtime_error("Invalid .BTF.ext section - wrong version");
  }
  if (bpf_ext_header.hdr_len > btf_ext.size()) {
    throw std::runtime_error(
        "Invalid .BTF.ext section - invalid header length");
  }

  size_t line_info_start = static_cast<size_t>(bpf_ext_header.hdr_len) +
                           static_cast<size_t>(bpf_ext_header.line_info_off);
  size_t line_info_end =
      line_info_start + static_cast<size_t>(bpf_ext_header.line_info_len);

  validate_range(btf_ext, line_info_start, line_info_end);

  btf_ext_offset = line_info_start;
  uint32_t line_info_record_size = read_btf<uint32_t>(
      btf_ext, btf_ext_offset, line_info_start, line_info_end);
  if (line_info_record_size < sizeof(bpf_line_info_t)) {
    throw std::runtime_error(std::string(
        "Invalid .BTF.ext section - invalid line info record size"));
  }

// Suppress warning C4815 on MSVC
// section_info is declared on the stack, but its size depends on the number of
// elements in the section. This is not a problem, because the number the code
// only uses the header.
#pragma warning(push)
#pragma warning(disable : 4815)
  for (; btf_ext_offset < line_info_end;) {
    auto section_info = read_btf<btf_ext_info_sec_t>(
        btf_ext, btf_ext_offset, line_info_start, line_info_end);
    auto section_name =
        _btf_find_string(string_table, section_info.sec_name_off);
    for (size_t index = 0; index < section_info.num_info; index++) {
      auto btf_line_info = read_btf<bpf_line_info_t>(
          btf_ext, btf_ext_offset, line_info_start, line_info_end);
      auto file_name =
          _btf_find_string(string_table, btf_line_info.file_name_off);
      auto source = _btf_find_string(string_table, btf_line_info.line_off);
      visitor(section_name, btf_line_info.insn_off, file_name, source,
              BPF_LINE_INFO_LINE_NUM(btf_line_info.line_col),
              BPF_LINE_INFO_LINE_COL(btf_line_info.line_col));
    }
  }
#pragma warning(pop)
}

void btf_parse_types(const std::vector<std::byte> &btf,
                     btf_type_visitor visitor) {
  std::map<size_t, std::string> string_table = _btf_parse_string_table(btf);
  btf_type_id id = 0;
  size_t offset = 0;

  auto btf_header = read_btf<btf_header_t>(btf, offset);

  if (btf_header.magic != BTF_HEADER_MAGIC) {
    throw std::runtime_error("Invalid .BTF section - wrong magic");
  }

  if (btf_header.version != BTF_HEADER_VERSION) {
    throw std::runtime_error("Invalid .BTF section - wrong version");
  }

  if (btf_header.hdr_len < sizeof(btf_header_t)) {
    throw std::runtime_error("Invalid .BTF section - wrong size");
  }

  size_t type_start = static_cast<size_t>(btf_header.hdr_len) +
                      static_cast<size_t>(btf_header.type_off);
  size_t type_end = type_start + static_cast<size_t>(btf_header.type_len);

  validate_range(btf, type_start, type_end);

  btf_kind_void kind_null;
  visitor(0, "void", {kind_null});

  for (offset = type_start; offset < type_end;) {
    std::optional<std::string> name;
    auto btf_type = read_btf<btf_type_t>(btf, offset, type_start, type_end);
    if (btf_type.name_off) {
      name = _btf_find_string(string_table, btf_type.name_off);
    } else {
      // Throw for types that should have a name.
      switch (BPF_TYPE_INFO_KIND(btf_type.info)) {
      case BTF_KIND_INT:
      case BTF_KIND_FWD:
      case BTF_KIND_TYPEDEF:
      case BTF_KIND_FUNCTION:
      case BTF_KIND_VAR:
      case BTF_KIND_DATA_SECTION:
      case BTF_KIND_FLOAT:
      case BTF_KIND_DECL_TAG:
      case BTF_KIND_TYPE_TAG:
        throw std::runtime_error("Invalid .BTF section - missing name");
      default:
        name = std::nullopt;
        break;
      }
    }
    btf_kind kind;
    switch (BPF_TYPE_INFO_KIND(btf_type.info)) {
    case BTF_KIND_INT: {
      btf_kind_int kind_int;
      uint32_t int_data = read_btf<uint32_t>(btf, offset, type_start, type_end);
      uint32_t encoding = BTF_INT_ENCODING(int_data);
      kind_int.offset_from_start_in_bits = BTF_INT_OFFSET(int_data);
      kind_int.field_width_in_bits = BTF_INT_BITS(int_data);
      kind_int.is_signed = BTF_INT_SIGNED & encoding;
      kind_int.is_bool = BTF_INT_BOOL & encoding;
      kind_int.is_char = BTF_INT_CHAR & encoding;
      kind_int.size_in_bytes = btf_type.size;
      kind_int.name = name.value();
      kind = kind_int;
      break;
    }
    case BTF_KIND_PTR: {
      btf_kind_ptr kind_ptr;
      kind_ptr.type = btf_type.type;
      kind = kind_ptr;
      break;
    }
    case BTF_KIND_ARRAY: {
      auto btf_array = read_btf<btf_array_t>(btf, offset, type_start, type_end);
      btf_kind_array kind_array;
      kind_array.element_type = btf_array.type;
      kind_array.index_type = btf_array.index_type;
      kind_array.count_of_elements = btf_array.nelems;
      kind = kind_array;
      break;
    }
    case BTF_KIND_STRUCT: {
      uint32_t member_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      btf_kind_struct kind_struct;
      for (uint32_t index = 0; index < member_count; index++) {
        btf_kind_struct_member member;
        auto btf_member =
            read_btf<btf_member_t>(btf, offset, type_start, type_end);
        if (btf_member.name_off) {
          member.name = _btf_find_string(string_table, btf_member.name_off);
        }
        member.type = btf_member.type;
        member.offset_from_start_in_bits = btf_member.offset;
        kind_struct.members.push_back(member);
      }
      kind_struct.size_in_bytes = btf_type.size;
      kind_struct.name = name;
      kind = kind_struct;
      break;
    }
    case BTF_KIND_UNION: {
      uint32_t member_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      btf_kind_union kind_union;
      for (uint32_t index = 0; index < member_count; index++) {
        btf_kind_struct_member member;
        auto btf_member =
            read_btf<btf_member_t>(btf, offset, type_start, type_end);
        if (btf_member.name_off) {
          member.name = _btf_find_string(string_table, btf_member.name_off);
        }
        member.type = btf_member.type;
        member.offset_from_start_in_bits = btf_member.offset;
        kind_union.members.push_back(member);
      }
      kind_union.name = name;
      kind_union.size_in_bytes = btf_type.size;
      kind = kind_union;
      break;
    }
    case BTF_KIND_ENUM: {
      uint32_t enum_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      btf_kind_enum kind_enum;
      for (uint32_t index = 0; index < enum_count; index++) {
        auto btf_enum = read_btf<btf_enum_t>(btf, offset, type_start, type_end);
        btf_kind_enum_member member;
        if (!btf_enum.name_off) {
          throw std::runtime_error(
              "Invalid .BTF section - invalid BTF_KIND_ENUM member name");
        }
        member.name = _btf_find_string(string_table, btf_enum.name_off);
        member.value = btf_enum.val;
        kind_enum.members.push_back(member);
      }
      kind_enum.is_signed = BPF_TYPE_INFO_KIND_FLAG(btf_type.info);
      kind_enum.name = name;
      kind_enum.size_in_bytes = btf_type.size;
      kind = kind_enum;
      break;
    }
    case BTF_KIND_FWD: {
      btf_kind_fwd kind_fwd;
      kind_fwd.name = name.value();
      kind_fwd.is_struct = BPF_TYPE_INFO_KIND_FLAG(btf_type.info);
      kind = kind_fwd;
      break;
    }
    case BTF_KIND_TYPEDEF: {
      btf_kind_typedef kind_typedef;
      kind_typedef.name = name.value();
      kind_typedef.type = btf_type.type;
      kind = kind_typedef;
      break;
    }
    case BTF_KIND_VOLATILE: {
      btf_kind_volatile kind_volatile;
      kind_volatile.type = btf_type.type;
      kind = kind_volatile;
      break;
    }
    case BTF_KIND_CONST: {
      btf_kind_const kind_const;
      kind_const.type = btf_type.type;
      kind = kind_const;
      break;
    }
    case BTF_KIND_RESTRICT: {
      btf_kind_restrict kind_restrict;
      kind_restrict.type = btf_type.type;
      kind = kind_restrict;
      break;
    }
    case BTF_KIND_FUNCTION: {
      btf_kind_function kind_function;
      kind_function.name = name.value();
      kind_function.type = btf_type.type;
      kind_function.linkage = static_cast<decltype(kind_function.linkage)>(
          BPF_TYPE_INFO_VLEN(btf_type.info));
      kind = kind_function;
      break;
    }
    case BTF_KIND_FUNCTION_PROTOTYPE: {
      btf_kind_function_prototype kind_function;
      uint32_t param_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      for (uint32_t index = 0; index < param_count; index++) {
        auto btf_param =
            read_btf<btf_param_t>(btf, offset, type_start, type_end);
        btf_kind_function_parameter param;
        // Name is optional.
        if (btf_param.name_off) {
          param.name = _btf_find_string(string_table, btf_param.name_off);
        }
        param.type = btf_param.type;
        kind_function.parameters.push_back(param);
      }
      kind_function.return_type = btf_type.type;
      kind = kind_function;
      break;
    }
    case BTF_KIND_VAR: {
      btf_kind_var kind_var;
      auto btf_var = read_btf<btf_var_t>(btf, offset, type_start, type_end);
      kind_var.name = name.value();
      kind_var.type = btf_type.type;
      kind_var.linkage =
          static_cast<decltype(btf_kind_var::linkage)>(btf_var.linkage);
      kind = kind_var;
      break;
    }
    case BTF_KIND_DATA_SECTION: {
      btf_kind_data_section kind_data_section;
      uint32_t section_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      for (uint32_t index = 0; index < section_count; index++) {
        auto btf_section_info =
            read_btf<btf_var_secinfo_t>(btf, offset, type_start, type_end);
        btf_kind_data_member member;
        member.type = btf_section_info.type;
        member.offset = btf_section_info.offset;
        member.size = btf_section_info.size;
        kind_data_section.members.push_back(member);
      }
      kind_data_section.name = name.value();
      kind_data_section.size = btf_type.size;
      kind = kind_data_section;
      break;
    }
    case BTF_KIND_FLOAT: {
      btf_kind_float kind_float;
      kind_float.name = name.value();
      kind_float.size_in_bytes = btf_type.size;
      kind = kind_float;
      break;
    }
    case BTF_KIND_DECL_TAG: {
      btf_kind_decl_tag kind_decl_tag;
      auto btf_decl_tag =
          read_btf<btf_decl_tag_t>(btf, offset, type_start, type_end);
      kind_decl_tag.name = name.value();
      kind_decl_tag.type = btf_type.type;
      kind_decl_tag.component_index = btf_decl_tag.component_idx;
      kind = kind_decl_tag;
      break;
    }
    case BTF_KIND_TYPE_TAG: {
      btf_kind_type_tag kind_type_tag;
      kind_type_tag.name = name.value();
      kind_type_tag.type = btf_type.type;
      kind = kind_type_tag;
      break;
    }
    case BTF_KIND_ENUM64: {
      uint32_t enum_count = BPF_TYPE_INFO_VLEN(btf_type.info);
      btf_kind_enum64 kind_enum;
      for (uint32_t index = 0; index < enum_count; index++) {
        auto btf_enum64 =
            read_btf<btf_enum64_t>(btf, offset, type_start, type_end);
        btf_kind_enum64_member member;
        member.name = _btf_find_string(string_table, btf_enum64.name_off);
        member.value = (static_cast<uint64_t>(btf_enum64.val_hi32) << 32) |
                       btf_enum64.val_lo32;
        kind_enum.members.push_back(member);
      }
      kind_enum.is_signed = BPF_TYPE_INFO_KIND_FLAG(btf_type.info);
      kind_enum.name = name;
      kind_enum.size_in_bytes = btf_type.size;
      kind = kind_enum;
      break;
    }
    default:
      throw std::runtime_error("Invalid .BTF section - invalid BTF_KIND - " +
                               std::to_string(kind.index()));
    }
    visitor(++id, name, kind);
  }
}
} // namespace libbtf