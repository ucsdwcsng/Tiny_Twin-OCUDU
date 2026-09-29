/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

openair0_device_t openair0;
int oai_exit=0;

void exit_function(const char* file, const char* function, const int line,const char* s, const int assert) {
   const char * msg= s==NULL ? "no comment": s;
   printf("Exiting at: %s:%d %s(), %s\n", file, line, function, msg);
   exit(-1);
}

extern unsigned char offset_mumimo_llr_drange_fix;
