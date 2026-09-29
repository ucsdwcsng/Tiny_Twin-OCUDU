/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief extern parameters
 */

#ifndef __OTG_EXTERNS_H__
#    define __OTG_EXTERNS_H__


/*!< \brief main log variables */
extern otg_t *g_otg; /*!< \brief global params */
extern otg_multicast_t *g_otg_multicast; /*!< \brief global params */
extern otg_info_t *otg_info; /*!< \brief info otg */
extern otg_multicast_info_t *otg_multicast_info; /*!< \brief  info otg: measurements about the simulation  */
extern otg_forms_info_t *otg_forms_info;

extern mapping otg_multicast_app_type_names[] ;

extern mapping otg_app_type_names[];

extern mapping otg_transport_protocol_names[];

extern mapping otg_ip_version_names[];

extern mapping otg_multicast_app_type_names[];

extern mapping otg_distribution_names[];

extern mapping frame_type_names[];

extern mapping switch_names[] ;

extern mapping packet_gen_names[];

#endif

