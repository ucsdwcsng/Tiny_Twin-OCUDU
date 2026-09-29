// Copyright (c) Microsoft Corporation
// SPDX-License-Identifier: MIT

#include "btf.h"
#include "btf_json.h"
#include "btf_map.h"
#include "btf_parse.h"
#include "btf_type_data.h"
#include "btf_write.h"
#include "options.h"

// Suppress some W4 warnings from the elfio library.
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4458)
#include "elfio/elfio.hpp"
#pragma warning(pop)

#include <functional>
#include <map>
#include <optional>
#include <sstream>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <tuple>
#include <vector>

int main(int argc, char **argv) {
  try {
    std::string input_file;
    std::string output_file;
    std::string json_output_file;
    std::string root_type;
    std::string c_output_file;
    bool pretty_print_json = false;

    options opts;
    opts.add(
        "-i", 2, [&](auto iter) { input_file = *iter; }, "Input file");
    opts.add(
        "-o", 2, [&](auto iter) { output_file = *iter; }, "Output file");
    opts.add(
        "-j", 2, [&](auto iter) { json_output_file = *iter; },
        "JSON output file");
    opts.add(
        "-r", 2, [&](auto iter) { root_type = *iter; }, "Root type");
    opts.add(
        "-p", 1, [&](auto iter) { pretty_print_json = true; },
        "Pretty print JSON");
    opts.add(
        "-t", 2, [&](auto iter) { c_output_file = *iter; }, "C output file");

    opts.parse(argc, argv);

    if (input_file.empty()) {
      std::cerr << "No input file" << std::endl;
      opts.print_help();
      return 1;
    }

    if (output_file.empty() && json_output_file.empty() &&
        c_output_file.empty()) {
      std::cerr << "No output file" << std::endl;
      opts.print_help();
      return 1;
    }

    // Read ELF file
    ELFIO::elfio reader;
    if (!reader.load(input_file)) {
      throw std::runtime_error("Failed to load ELF file: " + input_file);
    }

    auto btf_section = reader.sections[".BTF"];

    if (!btf_section) {
      throw std::runtime_error("Failed to find BTF section");
    }

    // Dump BTF section
    if (output_file.size() > 0) {
      std::ofstream output(output_file, std::ios::binary);
      if (!output) {
        throw std::runtime_error("Failed to open output file: " + output_file);
      }

      output.write((const char *)btf_section->get_data(),
                   btf_section->get_size());
      output.close();
    }

    // Parse BTF section
    libbtf::btf_type_data btf_data = std::vector<std::byte>(
        reinterpret_cast<const std::byte *>(btf_section->get_data()),
        reinterpret_cast<const std::byte *>(btf_section->get_data() +
                                            btf_section->get_size()));

    std::optional<std::function<bool(libbtf::btf_type_id)>> filter =
        std::nullopt;
    libbtf::btf_type_id root_type_id = 0;
    if (root_type.size() > 0) {
      root_type_id = std::strtoul(root_type.c_str(), nullptr, 10);
      if (root_type_id == 0) {
        root_type_id = btf_data.get_id(root_type);
      }

      if (root_type_id == 0) {
        throw std::runtime_error("Failed to find root type: " + root_type);
      }
      filter = [&](libbtf::btf_type_id type_id) {
        return type_id == root_type_id;
      };
    }

    if (json_output_file.size() > 0) {

      std::ofstream json_output;
      if (json_output_file != "-") {
        json_output.open(json_output_file);
        if (!json_output) {
          throw std::runtime_error("Failed to open JSON output file: " +
                                   json_output_file);
        }
      }
      std::ostream &out = json_output_file == "-" ? std::cout : json_output;

      if (pretty_print_json) {
        std::ostringstream oss;
        btf_data.to_json(oss, filter);
        out << libbtf::pretty_print_json(oss.str()) << std::endl;
      } else {
        btf_data.to_json(out, filter);
      }
    }

    if (c_output_file.size() > 0) {
      std::ofstream c_output;
      if (c_output_file != "-") {
        c_output.open(c_output_file);
        if (!c_output) {
          throw std::runtime_error("Failed to open C output file: " +
                                   c_output_file);
        }
        c_output << "// Generated from " << input_file << "\n\n";
      }

      std::ostream &out = c_output_file == "-" ? std::cout : c_output;

      btf_data.to_c_header(out, filter);
    }

    return 0;
  } catch (const std::runtime_error &err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return 1;
  }
}