#pragma once

#include "jbpf_verifier.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

    jbpf_verifier_result_t
    srsran_verify(const char* filename, const char* section, const char* asmfile);

#ifdef __cplusplus
}
#endif

