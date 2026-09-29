/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "position_interface.h"
#include "executables/nr-uesoftmodem.h"
#include "common/config/config_userapi.h"

static void read_position_coordinates(char *sectionName, position_t *position)
{
  paramdef_t position_params[] = POSITION_CONFIG_PARAMS_DEF;
  int ret = config_get(config_get_if(), position_params, sizeofArray(position_params), sectionName);
  AssertFatal(ret >= 0, "configuration couldn't be performed for position name: %s", sectionName);
}

void get_position_coordinates(int Mod_id, position_t *position)
{
  AssertFatal(Mod_id < NB_UE_INST, "Mod_id must be less than NB_UE_INST. Mod_id:%d NB_UE_INST:%d", Mod_id, NB_UE_INST);
  char positionName[64];
  snprintf(positionName, sizeof(positionName), "position%d", Mod_id);
  read_position_coordinates(positionName, position);
}
