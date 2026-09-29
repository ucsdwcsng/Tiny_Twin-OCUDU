// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once
#include "btf.h"

#include <functional>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#define LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(INDEX, TYPE)                        \
  template <>                                                                  \
  inline TYPE btf_type_data::get_kind_type<TYPE>(btf_type_id id) const {       \
    auto kind = get_kind(id);                                                  \
    if (kind.index() != INDEX) {                                               \
      throw std::runtime_error(std::string("Wrong type: Expected ") +          \
                               std::to_string(INDEX) + " Actual " +            \
                               std::to_string(kind.index()));                  \
    }                                                                          \
    return std::get<INDEX>(get_kind(id));                                      \
  }

namespace libbtf {
/**
 * @brief A class to represent the type information in a BTF section.
 */
class btf_type_data {
public:
  /**
   * @brief Construct a new empty btf type data object
   */
  btf_type_data() {
    // Add the void type.
    id_to_kind[0] = btf_kind_void{};
  }

  /**
   * @brief Construct a new btf type data object from a vector of bytes.
   *
   * @param[in] btf_data The BTF data.
   */
  btf_type_data(const std::vector<std::byte> &btf_data);

  /**
   * @brief Destroy the btf type data object
   */
  ~btf_type_data() = default;

  /**
   * @brief Get btf_type_id from a name.
   *
   * @param[in] name Name of the type.
   * @return Type id of the type.
   */
  btf_type_id get_id(const std::string &name) const;

  btf_kind_index get_kind_index(btf_type_id id) const {
    return static_cast<btf_kind_index>(get_kind(id).index());
  }

  /**
   * @brief Get the kind object as a specific type from a type id.
   *
   * @tparam T The type of the kind.
   * @param[in] id The id of the type.
   * @return The kind of the type as the specific type.
   */
  template <class T> T get_kind_type(btf_type_id id) const;

  btf_type_id dereference_pointer(btf_type_id id) const;
  uint32_t get_size(btf_type_id id) const;
  void
  to_json(std::ostream &out,
          std::optional<std::function<bool(btf_type_id)>> = std::nullopt) const;
  std::vector<std::byte> to_bytes() const;
  btf_type_id append(const btf_kind &kind);
  void replace(btf_type_id id, const btf_kind &kind);
  btf_type_id last_type_id() const { return id_to_kind.rbegin()->first; }

  void visit_depth_first(std::optional<std::function<bool(btf_type_id)>> before,
                         std::optional<std::function<void(btf_type_id)>> after,
                         btf_type_id id) const;

  void to_c_header(std::ostream &out,
                   std::optional<std::function<bool(btf_type_id)>> filter =
                       std::nullopt) const;

  std::vector<btf_type_id>
  dependency_order(std::optional<std::function<bool(btf_type_id)>> filter =
                       std::nullopt) const;

private:
  /**
   * @brief Get the kind object from a type id.
   *
   * @param[in] id Id of the type.
   * @return The kind of the type.
   */
  btf_kind get_kind(btf_type_id id) const;

  void update_name_to_id(btf_type_id id);
  void validate_type_graph(btf_type_id id,
                           std::set<btf_type_id> &visited) const;

  std::string get_type_name(btf_type_id id) const;
  std::string get_qualified_type_name(btf_type_id id) const;
  btf_type_id get_descendant_type_id(btf_type_id id) const;
  std::string get_type_declaration(btf_type_id id, const std::string &name,
                                   size_t indent) const;
  std::map<btf_type_id, btf_kind> id_to_kind;
  std::map<std::string, btf_type_id> name_to_id;
};

LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_VOID, btf_kind_void)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_INT, btf_kind_int)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_PTR, btf_kind_ptr)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_ARRAY, btf_kind_array)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_STRUCT, btf_kind_struct)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_UNION, btf_kind_union)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_ENUM, btf_kind_enum)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_FWD, btf_kind_fwd)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_TYPEDEF, btf_kind_typedef)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_VOLATILE, btf_kind_volatile)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_CONST, btf_kind_const)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_RESTRICT, btf_kind_restrict)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_FUNCTION, btf_kind_function)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_FUNCTION_PROTOTYPE,
                                   btf_kind_function_prototype)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_VAR, btf_kind_var)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_DATA_SECTION, btf_kind_data_section)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_FLOAT, btf_kind_float)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_DECL_TAG, btf_kind_decl_tag)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_TYPE_TAG, btf_kind_type_tag)
LIBBTF_BTF_TYPE_DATA_GET_KIND_TYPE(BTF_KIND_ENUM64, btf_kind_enum64)
} // namespace libbtf