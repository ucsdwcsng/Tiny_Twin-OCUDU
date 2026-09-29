/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include <vector>
#include <iostream>
#include "imscope_internal.h"

static extended_kpi_ue kpiStructure;

extern "C" {
static softmodem_params_t softmodem_params;
softmodem_params_t *get_softmodem_params(void)
{
  return &softmodem_params;
}
extended_kpi_ue *getKPIUE(void)
{
  return &kpiStructure;
}
}

void *imscope_thread(void *data_void_ptr);


std::vector<IQData> LoadFromFile(char *filename) {
  std::ifstream file(filename, std::ios::binary);
  std::vector<IQData> iq_data;
  if (file.is_open()) {
    while (file.peek() != EOF) {
      ImScopeData source;
      source.read_from_file(file);
      std::cout << "Loaded from IQ from file: " << source << std::endl;
      IQData iq;
      iq.Collect(&source, 0, 0);
      iq_data.push_back(iq);
    }
  }
  return iq_data;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  
  std::vector<IQData> iq_data = LoadFromFile(argv[1]);
  if (iq_data.size() == 0) {
    printf("Error: No IQ data found in file\n");
    return 1;
  }

  imscope_thread(static_cast<void *>(&iq_data));
}
