/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief: implementation of telnet commands related to softmodem linux process
 */

#define _GNU_SOURCE 
#include <string.h>
#include <pthread.h>


#define TELNETSERVERCODE
#include "telnetsrv.h"
#define TELNETSRV_LOADER_MAIN
#include "telnetsrv_loader.h"


int loader_show_cmd(char *buff, int debug, telnet_printfunc_t prnt);
telnetshell_cmddef_t loader_cmdarray[] = {
    {"show", "[params,modules]", loader_show_cmd, {(webfunc_t)loader_show_cmd}, 0, NULL},
    {"", "", NULL, {NULL}, 0, NULL},
};

/*-------------------------------------------------------------------------------------*/
int loader_show_cmd(char *buff, int debug, telnet_printfunc_t prnt)
{
  if (buff == NULL) {
    prnt("ERROR wrong loader SHOW command...\n");
    return 0;
  }
   if (debug > 0)
       prnt( "loader_show_cmd received %s\n",buff);

      if (strcasestr(buff,"params") != NULL) {
          prnt( "loader parameters:\n");
          prnt( "   Main executable build version: \"%s\"\n", loader_data.mainexec_buildversion);
          prnt( "   Default shared lib path: \"%s\"\n", loader_data.shlibpath);
          prnt( "   Max number of shared lib : %i\n", loader_data.maxshlibs);
      } else if (strcasestr(buff, "modules") != NULL || buff[0] == 0 || strcasestr(buff, "show") != NULL) {
        prnt("%i shared lib have been dynamicaly loaded by the oai loader\n", loader_data.numshlibs);
        for (int i = 0; i < loader_data.numshlibs; i++) {
          prnt("   Module %i: %s\n", i, loader_data.shlibs[i].name);
          prnt("       Shared library build version: \"%s\"\n", ((loader_data.shlibs[i].shlib_buildversion == NULL) ? "" : loader_data.shlibs[i].shlib_buildversion));
          prnt("       Shared library path: \"%s\"\n", loader_data.shlibs[i].thisshlib_path);
          prnt("       %i function pointers registered:\n", loader_data.shlibs[i].numfunc);
          for (int j = 0; j < loader_data.shlibs[i].numfunc; j++) {
            prnt("          function %i %s at %p\n", j, loader_data.shlibs[i].funcarray[j].fname, loader_data.shlibs[i].funcarray[j].fptr);
          }
        }
      } else {
        prnt("%s: wrong loader command...\n", buff);
      }
   return 0;
}

void add_loader_cmds(void)
{

   add_telnetcmd("loader", loader_globalvardef, loader_cmdarray);
}
