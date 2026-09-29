/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __POSITION_INTERFACE__H__
#define __POSITION_INTERFACE__H__

/* position configuration parameters name */
#define CONFIG_STRING_POSITION_X "x"
#define CONFIG_STRING_POSITION_Y "y"
#define CONFIG_STRING_POSITION_Z "z"

#define HELP_STRING_POSITION "Postion coordinates (x, y, z) config params"

// Position parameters config
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                            position configuration          parameters */
/*   optname                                         helpstr            paramflags    XXXptr              defXXXval type numelt */
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/
// clang-format off
#define POSITION_CONFIG_PARAMS_DEF { \
  {CONFIG_STRING_POSITION_X,  HELP_STRING_POSITION, 0,        .dblptr=&(position->X),                 .defuintval=0,           TYPE_DOUBLE,     0},    \
  {CONFIG_STRING_POSITION_Y,  HELP_STRING_POSITION, 0,        .dblptr=&(position->Y),                 .defuintval=0,           TYPE_DOUBLE,     0},    \
  {CONFIG_STRING_POSITION_Z,  HELP_STRING_POSITION, 0,        .dblptr=&(position->Z),                 .defuintval=0,           TYPE_DOUBLE,     0}     \
}
// clang-format on

typedef struct position {
  double X;
  double Y;
  double Z;
} position_t;

void get_position_coordinates(int Mod_id, position_t *position);

#endif
