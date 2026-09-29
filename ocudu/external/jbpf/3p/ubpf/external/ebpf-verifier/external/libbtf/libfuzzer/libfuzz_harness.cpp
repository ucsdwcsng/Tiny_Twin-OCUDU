// Copyright (c) Microsoft Corporation
// SPDX-License-Identifier: MIT

#include "btf.h"
#include "btf_json.h"
#include "btf_map.h"
#include "btf_parse.h"
#include "btf_type_data.h"
#include "btf_write.h"
#include "elfio/elfio.hpp"

#include <sstream>
#include <stddef.h>
#include <stdint.h>
#include <string>

extern "C" int LLVMFuzzerInitialize(int *, char ***) { return 0; }

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::string input(reinterpret_cast<const char *>(data), size);
  std::istringstream iss(input);

  ELFIO::elfio reader;
  if (!reader.load(iss)) {
    return 1;
  }

  auto btf = reader.sections[".BTF"];
  if (!btf) {
    return 1;
  }

  try {
    libbtf::btf_type_data btf_data = std::vector<std::byte>(
        {reinterpret_cast<const std::byte *>(data),
         reinterpret_cast<const std::byte *>(data + size)});
  } catch (const std::runtime_error &) {
    return 1;
  }

  return 0;
}