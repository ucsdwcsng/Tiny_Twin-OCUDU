/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief  Internal slice helper functions
 */

#ifndef __SLICING_INTERNAL_H__
#define __SLICING_INTERNAL_H__

#include "slicing.h"

void slicing_add_UE(slice_info_t *si, int UE_id);

void _remove_UE(slice_t **s, uint8_t *assoc, int UE_id);
void slicing_remove_UE(slice_info_t *si, int UE_id);

void _move_UE(slice_t **s, uint8_t *assoc, int UE_id, int to);
void slicing_move_UE(slice_info_t *si, int UE_id, int idx);

slice_t *_add_slice(uint8_t *n, slice_t **s);
slice_t *_remove_slice(uint8_t *n, slice_t **s, uint8_t *assoc, int idx);

#endif /* __SLICING_INTERNAL_H__ */
