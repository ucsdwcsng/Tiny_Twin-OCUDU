/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// Linear to ALaw
// 16 bit signed integer (typecasted as uint16_t) to 8 bit unsigned integer
extern const uint8_t lin2alaw_if4p5[65536];

// ALaw to Linear
// 8 bit unsigned integer to 16 bit signed integer (typecasted as uint16_t)
extern const uint16_t alaw2lin_if4p5[256];

extern const uint8_t lin2alaw_if5[65536];

extern const uint16_t alaw2lin_if5[256];
