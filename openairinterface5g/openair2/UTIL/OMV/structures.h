/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief structures used for the
*/

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "common/openairinterface5g_limits.h"
#ifndef __PHY_IMPLEMENTATION_DEFS_H__
	#define Maxneighbor NUMBER_OF_UE_MAX
	#ifndef NB_ANTENNAS_RX
		#define NB_ANTENNAS_RX  4
	#endif
#endif
//

// how to add an underlying map as OMV background

typedef struct Geo {
  int x, y,z;
  //int Speedx, Speedy, Speedz; // speeds in each of direction
  int mobility_type; // model of mobility
  int node_type;
  int Neighbors; // number of neighboring nodes (distance between the node and its neighbors < 100)
  int Neighbor[NUMBER_OF_UE_MAX]; // array of its neighbors
  //relevant to UE only
  unsigned short state;
  unsigned short rnti;
  unsigned int connected_eNB;
  int RSSI[NB_ANTENNAS_RX];
  int RSRP;
  int RSRQ;
  int Pathloss;
  /// more info to display
} Geo;

typedef struct Data_Flow_Unit {
  // int total_num_nodes;
  struct Geo geo[NUMBER_OF_eNB_MAX+NUMBER_OF_UE_MAX];
  int end;
} Data_Flow_Unit;


#endif // STRUCTURES_H
