
#include <iostream>
#include <chrono>

#include "srsran_verifier.hpp"
#include "jbpf_helper_api_defs_ext.h"
#include "context_descriptors.hpp"
#include "jbpf_srsran_contexts.h"



using std::string;
using std::vector;



jbpf_verifier_result_t
srsran_verify(const char* objfile, const char* section, const char* asmfile)
{
    // register jbpf_ran_ofh program type
    {
        // Next, we define the new program type
        EbpfProgramType new_program_type;
        new_program_type.name = "jbpf_ran_ofh";
        new_program_type.context_descriptor = &jbpf_ran_ofh_descr;
        new_program_type.section_prefixes = {
            "jbpf_ran_ofh"}; // The name(s) used to define the ELF section,
                               // where the codelet function is stored for this program type
        new_program_type.is_privileged = false;
        jbpf_verifier_register_program_type(JBPF_PROG_TYPE_RAN_OFH, new_program_type);
    }

    // register jbpf_ran_layer2 program type
    {
        // Next, we define the new program type
        EbpfProgramType new_program_type;
        new_program_type.name = "jbpf_ran_layer2";
        new_program_type.context_descriptor = &jbpf_ran_layer2_descr;
        new_program_type.section_prefixes = {
            "jbpf_ran_layer2"}; // The name(s) used to define the ELF section,
                               // where the codelet function is stored for this program type
        new_program_type.is_privileged = false;
        jbpf_verifier_register_program_type(JBPF_PROG_TYPE_RAN_LAYER2, new_program_type);
    }

    // register jbpf_mac_sched_ctx program type
    {
        // Next, we define the new program type
        EbpfProgramType new_program_type;
        new_program_type.name = "jbpf_ran_mac_sched";
        new_program_type.context_descriptor = &jbpf_ran_mac_sched_descr;
        new_program_type.section_prefixes = {
            "jbpf_ran_mac_sched"}; // The name(s) used to define the ELF section,
                               // where the codelet function is stored for this program type
        new_program_type.is_privileged = false;
        jbpf_verifier_register_program_type(JBPF_PROG_TYPE_RAN_MAC_SCHED, new_program_type);
    }

    // register jbpf_rlc_ctx program type
    {
        // Next, we define the new program type
        EbpfProgramType new_program_type;
        new_program_type.name = "jbpf_srsran_generic";
        new_program_type.context_descriptor = &jbpf_srsran_generic_descr;
        new_program_type.section_prefixes = {
            "jbpf_srsran_generic"}; // The name(s) used to define the ELF section,
                                // where the codelet function is stored for this program type
        new_program_type.is_privileged = false;
        jbpf_verifier_register_program_type(JBPF_PROG_TYPE_RAN_GENERIC, new_program_type);
    }
    
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ret = jbpf_verify(objfile, section, asmfile);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t1_t2_secs = t2 - t1;
    std::cout << "srsRAN Jbpf Verifier: file: " << objfile << ", verification time = " << t1_t2_secs.count() << " secs" << std::endl;
    return ret;
}
