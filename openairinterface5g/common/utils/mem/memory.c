/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/utils/mem/oai_memory.h"
#include "common/utils/LOG/log.h"

char* memory_get_path_from_ueid(const char* dirname, const char* filename, int ueid)
{
  /* Get non-volatile data directory */
  const char* path = getenv(dirname);
  char buffer[2048];

  if (path == NULL) {
    path = getenv(DEFAULT_NAS_PATH);
  }

  if (path == NULL) {
    path = ".";
  }

  /* Append non-volatile data file name */
  if (snprintf(buffer, sizeof(buffer), "%s/%s%d", path, filename, ueid) < 0) {
    return NULL;
  }

  return strdup(buffer);
}

/****************************************************************************
 **                                                                        **
 ** Name:  memory_read()                                             **
 **                                                                        **
 ** Description: Reads data from a non-volatile data file                  **
 **                                                                        **
 ** Inputs:  datafile:  The absolute path to the data file         **
 **    size:    The size of the data to read               **
 **    Others:  None                                       **
 **                                                                        **
 ** Outputs:   data:    Pointer to the data read                   **
 **    Return:  RETURNerror, RETURNok                      **
 **    Others:  None                                       **
 **                                                                        **
 ***************************************************************************/
int memory_read(const char* datafile, void* data, size_t size)
{
  int rc = RETURNerror;

  /* Open the data file for reading operation */
  FILE* fp = fopen(datafile, "rb");

  if (fp != NULL) {
    /* Read data */
    size_t n = fread(data, size, 1, fp);

    if (n == 1) {
      rc = RETURNok;
    }

    /* Close the data file */
    fclose(fp);
  }

  return (rc);
}

/****************************************************************************
 **                                                                        **
 ** Name:  memory_write()                                            **
 **                                                                        **
 ** Description: Writes data to a non-volatile data file                   **
 **                                                                        **
 ** Inputs:  datafile:  The absolute path to the data file         **
 **    data:    Pointer to the data to write               **
 **    size:    The size of the data to write              **
 **    Others:  None                                       **
 **                                                                        **
 ** Outputs:   None                                                      **
 **    Return:  RETURNerror, RETURNok                      **
 **    Others:  None                                       **
 **                                                                        **
 ***************************************************************************/
int memory_write(const char* datafile, const void* data, size_t size)
{
  int rc = RETURNerror;

  /* Open the data file for writing operation */
  FILE* fp = fopen(datafile, "wb");

  if (fp != NULL) {
    /* Write data */
    size_t n = fwrite(data, size, 1, fp);

    if (n == 1) {
      rc = RETURNok;
    }

    /* Close the data file */
    fclose(fp);
  }

  return (rc);
}
