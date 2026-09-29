// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#include "btf_write.h"
#include "btf_c_type.h"

#include <cstring>

namespace libbtf {

static void write_btf(std::vector<std::byte> &btf, const auto &value) {
  size_t length = sizeof(value);
  size_t offset = btf.size();
  btf.resize(offset + length);
  memcpy(btf.data() + offset, &value, length);
}

class btf_string_table {
public:
  btf_string_table() { add(""); }
  ~btf_string_table() = default;

  uint32_t add(const std::optional<std::string> &str) {
    if (!str) {
      return 0;
    }

    auto it = offsets.find(*str);
    if (it != offsets.end()) {
      return it->second;
    }
    size_t offset = bytes.size();
    bytes.insert(
        bytes.end(), reinterpret_cast<const std::byte *>(str->data()),
        reinterpret_cast<const std::byte *>(str->data() + str->size()));
    bytes.push_back(std::byte{0});
    offsets[*str] = static_cast<uint32_t>(offset);
    return static_cast<uint32_t>(offset);
  }

  const std::vector<std::byte> &get_bytes() const { return bytes; }

private:
  std::vector<std::byte> bytes;
  std::map<std::string, uint32_t> offsets;
};

std::vector<std::byte> btf_write_types(const std::vector<btf_kind> &btf_kind) {
  std::vector<std::byte> btf;
  btf_string_table string_table;
  std::vector<std::byte> type_table_bytes;

  auto pack_btf_int_data = [](bool is_signed, bool is_char, bool is_bool,
                              size_t offset, size_t bits) {
    uint32_t value = 0;
    value |= is_signed ? BTF_INT_SIGNED : 0;
    value |= is_char ? BTF_INT_CHAR : 0;
    value |= is_bool ? BTF_INT_BOOL : 0;
    value = value << 24;
    value |= offset & UINT8_MAX << 16;
    value |= bits & UINT8_MAX;
    return value;
  };

  for (const auto &kind : btf_kind) {
    if (kind.index() == BTF_KIND_VOID) {
      continue;
    }
    // Write common BTF type header.
    std::visit(
        [&](const auto &value) {
          btf_type_t btf_type = {0};
          uint32_t vlen = 0;

          if constexpr (btf_kind_traits<decltype(value)>::has_optional_name ||
                        btf_kind_traits<decltype(value)>::has_name) {
            btf_type.name_off = string_table.add(value.name);
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_return_type) {
            btf_type.type = value.return_type;
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_type) {
            btf_type.type = value.type;
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_members) {
            vlen = static_cast<uint32_t>(value.members.size());
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_parameters) {
            vlen = static_cast<uint32_t>(value.parameters.size());
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_size_in_bytes) {
            btf_type.size = value.size_in_bytes;
          }
          if constexpr (btf_kind_traits<decltype(value)>::has_linkage) {
            vlen = value.linkage;
          }

          btf_type.info = (static_cast<uint8_t>(kind.index()) << 24) |
                          static_cast<uint16_t>(vlen);
          write_btf(type_table_bytes, btf_type);
        },
        kind);

    // Write BTF type-specific data.
    switch (kind.index()) {
    case BTF_KIND_INT: {
      const auto &int_type = std::get<BTF_KIND_INT>(kind);
      write_btf(type_table_bytes,
                pack_btf_int_data(int_type.is_signed, int_type.is_char,
                                  int_type.is_bool,
                                  int_type.offset_from_start_in_bits,
                                  int_type.field_width_in_bits));
      break;
    }
    case BTF_KIND_ARRAY: {
      const auto &array_type = std::get<BTF_KIND_ARRAY>(kind);
      write_btf(type_table_bytes, btf_array_t{
                                      .type = array_type.element_type,
                                      .index_type = array_type.index_type,
                                      .nelems = array_type.count_of_elements,
                                  });
      break;
    }
    case BTF_KIND_STRUCT: {
      const auto &struct_type = std::get<BTF_KIND_STRUCT>(kind);
      for (const auto &member : struct_type.members) {
        write_btf(type_table_bytes,
                  btf_member_t{
                      .name_off = string_table.add(member.name),
                      .type = member.type,
                      .offset = member.offset_from_start_in_bits,
                  });
      }
      break;
    }
    case BTF_KIND_UNION: {
      const auto &union_type = std::get<BTF_KIND_UNION>(kind);
      for (const auto &member : union_type.members) {
        write_btf(type_table_bytes,
                  btf_member_t{
                      .name_off = string_table.add(member.name),
                      .type = member.type,
                      .offset = member.offset_from_start_in_bits,
                  });
      }
      break;
    }
    case BTF_KIND_ENUM: {
      const auto &enum_type = std::get<BTF_KIND_ENUM>(kind);
      for (const auto &member : enum_type.members) {
        write_btf(type_table_bytes,
                  btf_enum_t{
                      .name_off = string_table.add(member.name),
                      .val = member.value,
                  });
      }
      break;
    }
    case BTF_KIND_FUNCTION_PROTOTYPE: {
      const auto &func_proto_type = std::get<BTF_KIND_FUNCTION_PROTOTYPE>(kind);
      for (const auto &parameter : func_proto_type.parameters) {
        write_btf(type_table_bytes,
                  btf_param_t{
                      .name_off = string_table.add(parameter.name),
                      .type = parameter.type,
                  });
      }
      break;
    }
    case BTF_KIND_VAR: {
      const auto &var_type = std::get<BTF_KIND_VAR>(kind);
      write_btf(type_table_bytes, static_cast<uint32_t>(var_type.linkage));
      break;
    }
    case BTF_KIND_DATA_SECTION: {
      const auto &data_section_type = std::get<BTF_KIND_DATA_SECTION>(kind);
      for (const auto &member : data_section_type.members) {
        write_btf(type_table_bytes, btf_var_secinfo_t{
                                        .type = member.type,
                                        .offset = member.offset,
                                        .size = member.size,
                                    });
      }

      break;
    }
    case BTF_KIND_DECL_TAG: {
      const auto &decl_tag_type = std::get<BTF_KIND_DECL_TAG>(kind);
      write_btf(type_table_bytes,
                btf_decl_tag_t{.component_idx = decl_tag_type.component_index});
      break;
    }
    case BTF_KIND_ENUM64: {
      const auto &enum64_type = std::get<BTF_KIND_ENUM64>(kind);
      for (const auto &member : enum64_type.members) {
        btf_enum64_t enum_member = {0};
        enum_member.name_off = string_table.add(member.name);
        enum_member.val_lo32 = member.value & 0xFFFFFFFF;
        enum_member.val_hi32 = member.value >> 32;
        write_btf(type_table_bytes, enum_member);
      }
      break;
    }
    }
  }

  auto string_table_bytes = string_table.get_bytes();

  // Write the BTF header.
  write_btf(btf,
            btf_header_t{
                .magic = BTF_HEADER_MAGIC,
                .version = BTF_HEADER_VERSION,
                .flags = 0,
                .hdr_len = sizeof(btf_header_t),
                .type_off = 0,
                .type_len = static_cast<unsigned int>(type_table_bytes.size()),
                .str_off = static_cast<unsigned int>(type_table_bytes.size()),
                .str_len = static_cast<unsigned int>(string_table_bytes.size()),
            });

  btf.insert(btf.end(), type_table_bytes.begin(), type_table_bytes.end());
  btf.insert(btf.end(), string_table_bytes.begin(), string_table_bytes.end());

  return btf;
}
} // namespace libbtf