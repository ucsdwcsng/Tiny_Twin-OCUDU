/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

typedef unsigned char u8;

/*--------------------------- prototypes --------------------------*/
void f1    ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_a[8], const u8 op[16]);
void f2345 ( u8 k[16], u8 rand[16],
             u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6], const u8 op[16]);
void f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_s[8], const u8 op[16]);
void f5star( u8 k[16], u8 rand[16],
             u8 ak[6], const u8 op[16]);
void ComputeOPc(const u8 op[16], u8 op_c_pP[16] );
void RijndaelKeySchedule( u8 key[16] );
void RijndaelEncrypt(const u8 input[16], u8 output[16] );
