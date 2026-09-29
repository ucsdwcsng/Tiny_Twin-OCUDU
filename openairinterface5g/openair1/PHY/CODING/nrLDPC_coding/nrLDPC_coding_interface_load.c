/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! \file openair1/PHY/CODING/nrLDPC_coding/nrLDPC_coding_interface_load.c
 * \brief load library implementing coding/decoding algorithms
 */

#define _GNU_SOURCE 
#include <sys/types.h>
#include <stdlib.h>
#include <malloc.h>
#include "assertions.h"
#include "common/utils/LOG/log.h"
#include "PHY/CODING/nrLDPC_coding/nrLDPC_coding_interface.h"
#include "PHY/CODING/nrLDPC_extern.h"
#include "common/config/config_userapi.h"
#include "common/utils/load_module_shlib.h"

/* arguments used when called from phy simulators exec's which do not use the config module */
/* arg is used to initialize the config module so that the loader works as expected */
char *arguments_phy_simulators[64]={"ldpctest",NULL};

int load_nrLDPC_coding_interface(char *version, nrLDPC_coding_interface_t *itf, int max_num_pxsch)
{
  char *ptr = (char *)config_get_if();
  char libname[64] = "ldpc";

  if (ptr == NULL) { // phy simulators, config module possibly not loaded
    uniqCfg = load_configmodule(1, arguments_phy_simulators, CONFIG_ENABLECMDLINEONLY);
    logInit();
  }
  /* function description array, to be used when loading the encoding/decoding shared lib */
  loader_shlibfunc_t shlib_fdesc[] = {{.fname = "nrLDPC_coding_init"},
                                      {.fname = "nrLDPC_coding_shutdown"},
                                      {.fname = "nrLDPC_coding_decoder"},
                                      {.fname = "nrLDPC_coding_encoder"}};
  int ret;
  ret = load_module_version_shlib(libname, version, shlib_fdesc, sizeofArray(shlib_fdesc), NULL);
  if(ret < 0){
    LOG_D(PHY, "NR ULSCH decoding module unavailable");
    return ret;
  }
  itf->nrLDPC_coding_init = (nrLDPC_coding_init_t *)shlib_fdesc[0].fptr;
  itf->nrLDPC_coding_shutdown = (nrLDPC_coding_shutdown_t *)shlib_fdesc[1].fptr;
  itf->nrLDPC_coding_decoder = (nrLDPC_coding_decoder_t *)shlib_fdesc[2].fptr;
  itf->nrLDPC_coding_encoder = (nrLDPC_coding_encoder_t *)shlib_fdesc[3].fptr;

  AssertFatal(itf->nrLDPC_coding_init(max_num_pxsch) == 0, "error starting LDPC library %s %s\n", libname, version);

  return 0;
}

int free_nrLDPC_coding_interface(nrLDPC_coding_interface_t *interface)
{
  if (interface->nrLDPC_coding_shutdown) {
    return interface->nrLDPC_coding_shutdown();
  } else {
    return -1;
  }
}
