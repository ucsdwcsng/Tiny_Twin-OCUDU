/* SPDX-License-Identifier: LicenseRef-CSSL-1.0 */
#pragma once

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

class TapTraceFile {
 public:
  explicit TapTraceFile(const std::string &contents)
  {
    char name[] = "/tmp/oai-zmq-taps-XXXXXX";
    int fd = mkstemp(name);
    if (fd < 0)
      throw std::runtime_error("cannot create tap trace fixture");
    close(fd);
    path = name;
    std::ofstream(path) << contents;
  }
  ~TapTraceFile() { std::remove(path.c_str()); }
  TapTraceFile(const TapTraceFile &) = delete;
  TapTraceFile &operator=(const TapTraceFile &) = delete;
  std::string path;
};
