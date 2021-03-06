/*
 *  Copyright (c) 2017 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./vpx_dsp_rtcd.h"
#include "vp8/common/loopfilter.h"
#include "vp8/common/onyxc_int.h"
#include "vpx_ports/asmdefs_mmi.h"

DECLARE_ALIGNED(8, static const uint64_t, ff_ph_01) = { 0x0001000100010001ULL };
DECLARE_ALIGNED(8, static const uint64_t,
                ff_ph_003f) = { 0x003f003f003f003fULL };
DECLARE_ALIGNED(8, static const uint64_t,
                ff_ph_0900) = { 0x0900090009000900ULL };
DECLARE_ALIGNED(8, static const uint64_t,
                ff_ph_1200) = { 0x1200120012001200ULL };
DECLARE_ALIGNED(8, static const uint64_t,
                ff_ph_1b00) = { 0x1b001b001b001b00ULL };
DECLARE_ALIGNED(8, static const uint64_t, ff_pb_fe) = { 0xfefefefefefefefeULL };
DECLARE_ALIGNED(8, static const uint64_t, ff_pb_80) = { 0x8080808080808080ULL };
DECLARE_ALIGNED(8, static const uint64_t, ff_pb_04) = { 0x0404040404040404ULL };
DECLARE_ALIGNED(8, static const uint64_t, ff_pb_03) = { 0x0303030303030303ULL };
DECLARE_ALIGNED(8, static const uint64_t, ff_pb_01) = { 0x0101010101010101ULL };

void vp8_loop_filter_horizontal_edge_mmi(
    unsigned char *src_ptr, int src_pixel_step, const unsigned char *blimit,
    const unsigned char *limit, const unsigned char *thresh, int count) {
  uint32_t tmp[1];
  mips_reg addr[2];
  double ftmp[12];
  __asm__ volatile (
    "1:                                                             \n\t"
    "gsldlc1    %[ftmp10],  0x07(%[limit])                          \n\t"
    "gsldrc1    %[ftmp10],  0x00(%[limit])                          \n\t"

    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x4])
    "gsldlc1    %[ftmp1],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp1],   0x00(%[addr1])                          \n\t"

    MMI_SUBU(%[addr1], %[addr0], %[src_pixel_step_x4])
    "gsldlc1    %[ftmp3],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp3],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp0],   %[ftmp1],           %[ftmp3]            \n\t"
    "psubusb    %[ftmp0],   %[ftmp0],           %[ftmp10]           \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gsldlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp3],           %[ftmp4]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp5],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp9],   %[ftmp4],           %[ftmp5]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp9],           %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    "gsldlc1    %[ftmp6],   0x07(%[src_ptr])                        \n\t"
    "gsldrc1    %[ftmp6],   0x00(%[src_ptr])                        \n\t"

    "gsldlc1    %[ftmp7],   0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp7],   0x00(%[addr0])                          \n\t"
    "pasubub    %[ftmp11],  %[ftmp7],           %[ftmp6]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp11],          %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    MMI_ADDU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gsldlc1    %[ftmp8],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp8],           %[ftmp7]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step_x2])
    "gsldlc1    %[ftmp2],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp2],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp2],           %[ftmp8]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    "pasubub    %[ftmp1],   %[ftmp5],           %[ftmp6]            \n\t"
    "paddusb    %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    "pasubub    %[ftmp2],   %[ftmp4],           %[ftmp7]            \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ff_pb_fe]         \n\t"
    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp10]                               \n\t"
    "psrlh      %[ftmp2],   %[ftmp2],           %[ftmp10]           \n\t"
    "paddusb    %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "gsldlc1    %[ftmp10],  0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp10],  0x00(%[blimit])                         \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp10]           \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"
    "xor        %[ftmp10],  %[ftmp10],          %[ftmp10]           \n\t"
    "pcmpeqb    %[ftmp0],   %[ftmp0],           %[ftmp10]           \n\t"

    "gsldlc1    %[ftmp10],  0x07(%[thresh])                         \n\t"
    "gsldrc1    %[ftmp10],  0x00(%[thresh])                         \n\t"
    "psubusb    %[ftmp1],   %[ftmp9],           %[ftmp10]           \n\t"
    "psubusb    %[ftmp2],   %[ftmp11],          %[ftmp10]           \n\t"
    "paddb      %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "xor        %[ftmp2],   %[ftmp2],           %[ftmp2]            \n\t"
    "pcmpeqb    %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "pcmpeqb    %[ftmp2],   %[ftmp2],           %[ftmp2]            \n\t"
    "xor        %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"

    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"

    "psubsb     %[ftmp2],   %[ftmp4],           %[ftmp7]            \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ftmp1]            \n\t"
    "psubsb     %[ftmp3],   %[ftmp6],           %[ftmp5]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"

    "paddsb     %[ftmp8],   %[ftmp2],           %[ff_pb_03]         \n\t"
    "paddsb     %[ftmp9],   %[ftmp2],           %[ff_pb_04]         \n\t"

    "xor        %[ftmp0],   %[ftmp0],           %[ftmp0]            \n\t"
    "xor        %[ftmp11],  %[ftmp11],          %[ftmp11]           \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp0],           %[ftmp8]            \n\t"
    "punpckhbh  %[ftmp11],  %[ftmp11],          %[ftmp8]            \n\t"

    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp10]                               \n\t"
    "psrah      %[ftmp0],   %[ftmp0],           %[ftmp10]           \n\t"
    "psrah      %[ftmp11],  %[ftmp11],          %[ftmp10]           \n\t"
    "packsshb   %[ftmp8],   %[ftmp0],           %[ftmp11]           \n\t"
    "xor        %[ftmp0],   %[ftmp0],           %[ftmp0]            \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"
    "psrah      %[ftmp0],   %[ftmp0],           %[ftmp10]           \n\t"
    "xor        %[ftmp11],  %[ftmp11],          %[ftmp11]           \n\t"
    "punpckhbh  %[ftmp9],   %[ftmp11],          %[ftmp9]            \n\t"
    "psrah      %[ftmp9],   %[ftmp9],           %[ftmp10]           \n\t"
    "paddsh     %[ftmp11],  %[ftmp0],           %[ff_ph_01]         \n\t"
    "packsshb   %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"
    "paddsh     %[ftmp9],   %[ftmp9],           %[ff_ph_01]         \n\t"

    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp10]                               \n\t"
    "psrah      %[ftmp11],  %[ftmp11],          %[ftmp10]           \n\t"
    "psrah      %[ftmp9],   %[ftmp9],           %[ftmp10]           \n\t"
    "packsshb   %[ftmp11],  %[ftmp11],          %[ftmp9]            \n\t"
    "pandn      %[ftmp1],   %[ftmp1],           %[ftmp11]           \n\t"
    "paddsb     %[ftmp5],   %[ftmp5],           %[ftmp8]            \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gssdlc1    %[ftmp5],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "paddsb     %[ftmp4],   %[ftmp4],           %[ftmp1]            \n\t"
    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "gssdlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"

    "psubsb     %[ftmp6],   %[ftmp6],           %[ftmp0]            \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "gssdlc1    %[ftmp6],   0x07(%[src_ptr])                        \n\t"
    "gssdrc1    %[ftmp6],   0x00(%[src_ptr])                        \n\t"

    "psubsb     %[ftmp7],   %[ftmp7],           %[ftmp1]            \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"
    "gssdlc1    %[ftmp7],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp7],   0x00(%[addr0])                          \n\t"

    "addiu      %[count],   %[count],           -0x01               \n\t"
    MMI_ADDIU(%[src_ptr], %[src_ptr], 0x08)
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [ftmp10]"=&f"(ftmp[10]),            [ftmp11]"=&f"(ftmp[11]),
      [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),            [addr1]"=&r"(addr[1]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [limit]"r"(limit),                [blimit]"r"(blimit),
      [thresh]"r"(thresh),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [src_pixel_step_x2]"r"((mips_reg)(src_pixel_step<<1)),
      [src_pixel_step_x4]"r"((mips_reg)(src_pixel_step<<2)),
      [ff_ph_01]"f"(ff_ph_01),          [ff_pb_fe]"f"(ff_pb_fe),
      [ff_pb_80]"f"(ff_pb_80),          [ff_pb_04]"f"(ff_pb_04),
      [ff_pb_03]"f"(ff_pb_03)
    : "memory"
  );
}

void vp8_loop_filter_vertical_edge_mmi(unsigned char *src_ptr,
                                       int src_pixel_step,
                                       const unsigned char *blimit,
                                       const unsigned char *limit,
                                       const unsigned char *thresh, int count) {
  uint32_t tmp[1];
  mips_reg addr[2];
  double ftmp[13];

  __asm__ volatile (
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_ADDU(%[src_ptr], %[src_ptr], %[tmp0])
    MMI_SUBU(%[src_ptr], %[src_ptr], 0x04)

    "1:                                                             \n\t"
    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])

    MMI_SLL (%[tmp0], %[src_pixel_step], 0x01)
    MMI_ADDU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr1])                          \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[tmp0])
    "gsldlc1    %[ftmp12],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp1],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp2],   %[ftmp11],          %[ftmp12]           \n\t"

    "gsldlc1    %[ftmp11],  0x07(%[src_ptr])                        \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[src_ptr])                        \n\t"
    "gsldlc1    %[ftmp12],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr0])                          \n\t"
    "punpcklbh  %[ftmp3],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp4],   %[ftmp11],          %[ftmp12]           \n\t"

    "punpcklhw  %[ftmp5],   %[ftmp4],           %[ftmp2]            \n\t"
    "punpckhhw  %[ftmp6],   %[ftmp4],           %[ftmp2]            \n\t"
    "punpcklhw  %[ftmp7],   %[ftmp3],           %[ftmp1]            \n\t"
    "punpckhhw  %[ftmp8],   %[ftmp3],           %[ftmp1]            \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x01)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp12],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp9],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp10],  %[ftmp11],          %[ftmp12]           \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr1])                          \n\t"
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[addr0], %[tmp0])
    "gsldlc1    %[ftmp12],  0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp11],  %[ftmp11],          %[ftmp12]           \n\t"

    "punpcklhw  %[ftmp1],   %[ftmp11],          %[ftmp10]           \n\t"
    "punpckhhw  %[ftmp2],   %[ftmp11],          %[ftmp10]           \n\t"
    "punpcklhw  %[ftmp3],   %[ftmp0],           %[ftmp9]            \n\t"
    "punpckhhw  %[ftmp4],   %[ftmp0],           %[ftmp9]            \n\t"

    /* ftmp9:q0  ftmp10:q1 */
    "punpcklwd  %[ftmp9],   %[ftmp1],           %[ftmp5]            \n\t"
    "punpckhwd  %[ftmp10],  %[ftmp1],           %[ftmp5]            \n\t"
    /* ftmp11:q2  ftmp12:q3 */
    "punpcklwd  %[ftmp11],  %[ftmp2],           %[ftmp6]            \n\t"
    "punpckhwd  %[ftmp12],  %[ftmp2],           %[ftmp6]            \n\t"
    /* ftmp1:p3  ftmp2:p2 */
    "punpcklwd  %[ftmp1],   %[ftmp3],           %[ftmp7]            \n\t"
    "punpckhwd  %[ftmp2],   %[ftmp3],           %[ftmp7]            \n\t"
    /* ftmp5:p1  ftmp6:p0 */
    "punpcklwd  %[ftmp5],   %[ftmp4],           %[ftmp8]            \n\t"
    "punpckhwd  %[ftmp6],   %[ftmp4],           %[ftmp8]            \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[limit])                          \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[limit])                          \n\t"

    /* abs (q3-q2) */
    "pasubub    %[ftmp7],   %[ftmp12],          %[ftmp11]           \n\t"
    "psubusb    %[ftmp0],   %[ftmp7],           %[ftmp8]            \n\t"
    /* abs (q2-q1) */
    "pasubub    %[ftmp7],   %[ftmp11],          %[ftmp10]           \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* ftmp3: abs(q1-q0) */
    "pasubub    %[ftmp3],   %[ftmp10],          %[ftmp9]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp3],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* ftmp4: abs(p1-p0) */
    "pasubub    %[ftmp4],   %[ftmp5],           %[ftmp6]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp4],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* abs (p2-p1) */
    "pasubub    %[ftmp7],   %[ftmp2],           %[ftmp5]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* abs (p3-p2) */
    "pasubub    %[ftmp7],   %[ftmp1],           %[ftmp2]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[blimit])                         \n\t"

    /* abs (p0-q0) */
    "pasubub    %[ftmp11],  %[ftmp9],           %[ftmp6]            \n\t"
    "paddusb    %[ftmp11],  %[ftmp11],          %[ftmp11]           \n\t"
    /* abs (p1-q1) */
    "pasubub    %[ftmp12],  %[ftmp10],          %[ftmp5]            \n\t"
    "and        %[ftmp12],  %[ftmp12],          %[ff_pb_fe]         \n\t"
    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp1]                                \n\t"
    "psrlh      %[ftmp12],  %[ftmp12],          %[ftmp1]            \n\t"
    "paddusb    %[ftmp1],   %[ftmp11],          %[ftmp12]           \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"
    "xor        %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    /* ftmp0:mask */
    "pcmpeqb    %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[thresh])                         \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[thresh])                         \n\t"

    /* ftmp3: abs(q1-q0)  ftmp4: abs(p1-p0) */
    "psubusb    %[ftmp4],   %[ftmp4],           %[ftmp8]            \n\t"
    "psubusb    %[ftmp3],   %[ftmp3],           %[ftmp8]            \n\t"
    "or         %[ftmp2],   %[ftmp4],           %[ftmp3]            \n\t"
    "pcmpeqb    %[ftmp2],   %[ftmp2],           %[ftmp1]            \n\t"
    "pcmpeqb    %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    /* ftmp1:hev */
    "xor        %[ftmp1],   %[ftmp2],           %[ftmp1]            \n\t"

    "xor        %[ftmp10],  %[ftmp10],          %[ff_pb_80]         \n\t"
    "xor        %[ftmp9],   %[ftmp9],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"

    "psubsb     %[ftmp2],   %[ftmp5],           %[ftmp10]           \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ftmp1]            \n\t"
    "psubsb     %[ftmp3],   %[ftmp9],           %[ftmp6]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp3]            \n\t"
    /* ftmp2:filter_value */
    "and        %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"

    "paddsb     %[ftmp11],  %[ftmp2],           %[ff_pb_04]         \n\t"
    "paddsb     %[ftmp12],  %[ftmp2],           %[ff_pb_03]         \n\t"

    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp7]                                \n\t"
    "xor       %[ftmp0],    %[ftmp0],           %[ftmp0]            \n\t"
    "xor       %[ftmp8],    %[ftmp8],           %[ftmp8]            \n\t"
    "punpcklbh %[ftmp0],    %[ftmp0],           %[ftmp12]           \n\t"
    "punpckhbh %[ftmp8],    %[ftmp8],           %[ftmp12]           \n\t"
    "psrah     %[ftmp0],    %[ftmp0],           %[ftmp7]            \n\t"
    "psrah     %[ftmp8],    %[ftmp8],           %[ftmp7]            \n\t"
    "packsshb  %[ftmp12],   %[ftmp0],           %[ftmp8]            \n\t"

    "xor       %[ftmp0],    %[ftmp0],           %[ftmp0]            \n\t"
    "xor       %[ftmp8],    %[ftmp8],           %[ftmp8]            \n\t"
    "punpcklbh %[ftmp0],    %[ftmp0],           %[ftmp11]           \n\t"
    "punpckhbh %[ftmp8],    %[ftmp8],           %[ftmp11]           \n\t"
    "psrah     %[ftmp0],    %[ftmp0],           %[ftmp7]            \n\t"
    "psrah     %[ftmp8],    %[ftmp8],           %[ftmp7]            \n\t"
    "packsshb  %[ftmp11],   %[ftmp0],           %[ftmp8]            \n\t"

    "psubsb     %[ftmp9],   %[ftmp9],           %[ftmp11]           \n\t"
    "xor        %[ftmp9],   %[ftmp9],           %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp6],   %[ftmp6],           %[ftmp12]           \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "paddsh     %[ftmp0],   %[ftmp0],           %[ff_ph_01]         \n\t"
    "paddsh     %[ftmp8],   %[ftmp8],           %[ff_ph_01]         \n\t"

    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp7]                                \n\t"
    "psrah      %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    "psrah      %[ftmp8],   %[ftmp8],           %[ftmp7]            \n\t"
    "packsshb   %[ftmp2],   %[ftmp0],           %[ftmp8]            \n\t"
    "pandn      %[ftmp2],   %[ftmp1],           %[ftmp2]            \n\t"
    "psubsb     %[ftmp10],  %[ftmp10],          %[ftmp2]            \n\t"
    "xor        %[ftmp10],  %[ftmp10],          %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp5],   %[ftmp5],           %[ftmp2]            \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"

    /* ftmp5: *op1 ; ftmp6: *op0 */
    "punpcklbh  %[ftmp2],   %[ftmp5],           %[ftmp6]            \n\t"
    "punpckhbh  %[ftmp1],   %[ftmp5],           %[ftmp6]            \n\t"
    /* ftmp9: *oq0 ; ftmp10: *oq1 */
    "punpcklbh  %[ftmp4],   %[ftmp9],           %[ftmp10]           \n\t"
    "punpckhbh  %[ftmp3],   %[ftmp9],           %[ftmp10]           \n\t"
    "punpckhhw  %[ftmp6],   %[ftmp2],           %[ftmp4]            \n\t"
    "punpcklhw  %[ftmp2],   %[ftmp2],           %[ftmp4]            \n\t"
    "punpckhhw  %[ftmp5],   %[ftmp1],           %[ftmp3]            \n\t"
    "punpcklhw  %[ftmp1],   %[ftmp1],           %[ftmp3]            \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsswlc1    %[ftmp2],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp2],   0x02(%[addr1])                          \n\t"

    "li         %[tmp0],    0x20                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "dsrl       %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[addr0], %[tmp0])
    "gsswlc1    %[ftmp2],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp2],   0x02(%[addr1])                          \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x01)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsswlc1    %[ftmp6],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp6],   0x02(%[addr1])                          \n\t"

    "dsrl       %[ftmp6],   %[ftmp6],           %[ftmp9]            \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsswlc1    %[ftmp6],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp6],   0x02(%[addr1])                          \n\t"
    "gsswlc1    %[ftmp1],   0x05(%[src_ptr])                        \n\t"
    "gsswrc1    %[ftmp1],   0x02(%[src_ptr])                        \n\t"

    "dsrl       %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "gsswlc1    %[ftmp1],   0x05(%[addr0])                          \n\t"
    "gsswrc1    %[ftmp1],   0x02(%[addr0])                          \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step])
    "gsswlc1    %[ftmp5],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp5],   0x02(%[addr1])                          \n\t"

    "dsrl       %[ftmp5],   %[ftmp5],           %[ftmp9]            \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[tmp0])
    "gsswlc1    %[ftmp5],   0x05(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp5],   0x02(%[addr1])                          \n\t"

    MMI_ADDIU(%[count], %[count], -0x01)
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x03)
    MMI_ADDU(%[src_ptr], %[src_ptr], %[tmp0])
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [ftmp10]"=&f"(ftmp[10]),            [ftmp11]"=&f"(ftmp[11]),
      [ftmp12]"=&f"(ftmp[12]),            [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),            [addr1]"=&r"(addr[1]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [limit]"r"(limit),                [blimit]"r"(blimit),
      [thresh]"r"(thresh),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [ff_ph_01]"f"(ff_ph_01),          [ff_pb_03]"f"(ff_pb_03),
      [ff_pb_04]"f"(ff_pb_04),          [ff_pb_80]"f"(ff_pb_80),
      [ff_pb_fe]"f"(ff_pb_fe)
    : "memory"
  );
}

#define VP8_MBLOOP_HPSRAB                                               \
  "xor        %[ftmp3],   %[ftmp3],           %[ftmp3]            \n\t" \
  "xor        %[ftmp8],   %[ftmp8],           %[ftmp8]            \n\t" \
  "punpcklbh  %[ftmp3],   %[ftmp3],           %[ftmp0]            \n\t" \
  "punpckhbh  %[ftmp8],   %[ftmp8],           %[ftmp0]            \n\t" \
  "psrah      %[ftmp3],   %[ftmp3],           %[ftmp9]            \n\t" \
  "psrah      %[ftmp8],   %[ftmp8],           %[ftmp9]            \n\t" \
  "packsshb   %[ftmp0],   %[ftmp3],           %[ftmp8]            \n\t"

#define VP8_MBLOOP_HPSRAB_PMULHH(reg1, reg2) \
  "pmulhh   " #reg1 ",  " #reg1 ",  " #reg2 "                     \n\t"

#define VP8_MBLOOP_HPSRAB_ADD(reg) \
  "xor        %[ftmp3],   %[ftmp3],           %[ftmp3]            \n\t" \
  "xor        %[ftmp8],   %[ftmp8],           %[ftmp8]            \n\t" \
  "punpcklbh  %[ftmp3],   %[ftmp3],           %[ftmp2]            \n\t" \
  "punpckhbh  %[ftmp8],   %[ftmp8],           %[ftmp2]            \n\t" \
  VP8_MBLOOP_HPSRAB_PMULHH(%[ftmp3], reg)                               \
  VP8_MBLOOP_HPSRAB_PMULHH(%[ftmp8], reg)                               \
  "paddh      %[ftmp3],   %[ftmp3],           %[ff_ph_003f]       \n\t" \
  "paddh      %[ftmp8],   %[ftmp8],           %[ff_ph_003f]       \n\t" \
  "psrah      %[ftmp3],   %[ftmp3],           %[ftmp9]            \n\t" \
  "psrah      %[ftmp8],   %[ftmp8],           %[ftmp9]            \n\t" \
  "packsshb   %[ftmp3],   %[ftmp3],           %[ftmp8]            \n\t"

void vp8_mbloop_filter_horizontal_edge_mmi(
    unsigned char *src_ptr, int src_pixel_step, const unsigned char *blimit,
    const unsigned char *limit, const unsigned char *thresh, int count) {
  uint32_t tmp[1];
  mips_reg addr[2];
  DECLARE_ALIGNED(8, const uint64_t, srct[1]);
  double ftmp[10];

  __asm__ volatile (
    "1:                                                             \n\t"
    "gsldlc1    %[ftmp9],   0x07(%[limit])                          \n\t"
    "gsldrc1    %[ftmp9],   0x00(%[limit])                          \n\t"

    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp1],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp1],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[addr0], %[tmp0])
    "gsldlc1    %[ftmp3],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp3],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp0],   %[ftmp1],           %[ftmp3]            \n\t"
    "psubusb    %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"

    /* ftmp4:p1 */
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x01)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp3],           %[ftmp4]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    /* ftmp5:p0 */
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp5],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp4],           %[ftmp5]            \n\t"
    "sdc1       %[ftmp1],   0x00(%[srct])                           \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    /* ftmp6:q0 */
    "gsldlc1    %[ftmp6],   0x07(%[src_ptr])                        \n\t"
    "gsldrc1    %[ftmp6],   0x00(%[src_ptr])                        \n\t"

    /* ftmp7:q1 */
    "gsldlc1    %[ftmp7],   0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp7],   0x00(%[addr0])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp7],           %[ftmp6]            \n\t"
    "sdc1       %[ftmp1],   0x08(%[srct])                           \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    MMI_ADDU(%[addr1], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp8],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp8],           %[ftmp7]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    MMI_ADDU(%[addr1], %[addr0], %[tmp0])
    "gsldlc1    %[ftmp2],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp2],   0x00(%[addr1])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp2],           %[ftmp8]            \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    "pasubub    %[ftmp1],   %[ftmp5],           %[ftmp6]            \n\t"
    "paddusb    %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    "pasubub    %[ftmp2],   %[ftmp4],           %[ftmp7]            \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ff_pb_fe]         \n\t"
    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "psrlh      %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    "paddusb    %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "gsldlc1    %[ftmp9],   0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp9],   0x00(%[blimit])                         \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"
    "xor        %[ftmp9],   %[ftmp9],           %[ftmp9]            \n\t"
    /* ftmp0: mask */
    "pcmpeqb    %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"

    "gsldlc1    %[ftmp9],   0x07(%[thresh])                         \n\t"
    "gsldrc1    %[ftmp9],   0x00(%[thresh])                         \n\t"
    "ldc1       %[ftmp1],   0x00(%[srct])                           \n\t"
    "psubusb    %[ftmp1],   %[ftmp1],           %[ftmp9]            \n\t"
    "ldc1       %[ftmp2],   0x08(%[srct])                           \n\t"
    "psubusb    %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    "paddb      %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "xor        %[ftmp2],   %[ftmp2],           %[ftmp2]            \n\t"
    "pcmpeqb    %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"
    "pcmpeqb    %[ftmp2],   %[ftmp2],           %[ftmp2]            \n\t"
    /* ftmp1:hev*/
    "xor        %[ftmp1],   %[ftmp1],           %[ftmp2]            \n\t"

    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"

    "psubsb     %[ftmp2],   %[ftmp4],           %[ftmp7]            \n\t"
    "psubsb     %[ftmp9],   %[ftmp6],           %[ftmp5]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp9]            \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"
    "sdc1       %[ftmp2],   0x00(%[srct])                           \n\t"
    "and        %[ftmp2],   %[ftmp2],           %[ftmp1]            \n\t"

    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "paddsb     %[ftmp0],   %[ftmp2],           %[ff_pb_03]         \n\t"
    VP8_MBLOOP_HPSRAB
    "paddsb     %[ftmp5],   %[ftmp5],           %[ftmp0]            \n\t"
    "paddsb     %[ftmp0],   %[ftmp2],           %[ff_pb_04]         \n\t"
    VP8_MBLOOP_HPSRAB
    "psubsb     %[ftmp6],   %[ftmp6],           %[ftmp0]            \n\t"
    "ldc1       %[ftmp2],   0x00(%[srct])                           \n\t"
    "pandn      %[ftmp2],   %[ftmp1],           %[ftmp2]            \n\t"

    "li         %[tmp0],    0x07                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    VP8_MBLOOP_HPSRAB_ADD(%[ff_ph_1b00])
    "psubsb     %[ftmp6],   %[ftmp6],           %[ftmp3]            \n\t"
    "paddsb     %[ftmp5],   %[ftmp5],           %[ftmp3]            \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gssdlc1    %[ftmp5],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"
    "gssdlc1    %[ftmp6],   0x07(%[src_ptr])                        \n\t"
    "gssdrc1    %[ftmp6],   0x00(%[src_ptr])                        \n\t"
    VP8_MBLOOP_HPSRAB_ADD(%[ff_ph_1200])
    "paddsb     %[ftmp4],   %[ftmp4],           %[ftmp3]            \n\t"
    "psubsb     %[ftmp7],   %[ftmp7],           %[ftmp3]            \n\t"
    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"

    "gssdlc1    %[ftmp7],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp7],   0x00(%[addr0])                          \n\t"
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x01)
    MMI_SUBU(%[addr1], %[src_ptr], %[tmp0])
    "gssdlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"

    VP8_MBLOOP_HPSRAB_ADD(%[ff_ph_0900])
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr1], %[addr0], %[tmp0])
    "gsldlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step])
    "gsldlc1    %[ftmp7],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp7],   0x00(%[addr1])                          \n\t"

    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp4],   %[ftmp4],           %[ftmp3]            \n\t"
    "psubsb     %[ftmp7],   %[ftmp7],           %[ftmp3]            \n\t"
    "xor        %[ftmp4],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp7],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp7],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[addr0], %[tmp0])
    "gssdlc1    %[ftmp4],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp4],   0x00(%[addr1])                          \n\t"

    "addiu      %[count],   %[count],           -0x01               \n\t"
    MMI_ADDIU(%[src_ptr], %[src_ptr], 0x08)
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),            [addr1]"=&r"(addr[1]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [limit]"r"(limit),                [blimit]"r"(blimit),
      [srct]"r"(srct),                  [thresh]"r"(thresh),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [ff_pb_fe]"f"(ff_pb_fe),          [ff_pb_80]"f"(ff_pb_80),
      [ff_pb_04]"f"(ff_pb_04),          [ff_pb_03]"f"(ff_pb_03),
      [ff_ph_0900]"f"(ff_ph_0900),      [ff_ph_1b00]"f"(ff_ph_1b00),
      [ff_ph_1200]"f"(ff_ph_1200),      [ff_ph_003f]"f"(ff_ph_003f)
    : "memory"
  );
}

#define VP8_MBLOOP_VPSRAB_ADDH                                          \
  "xor        %[ftmp7],   %[ftmp7],           %[ftmp7]            \n\t" \
  "xor        %[ftmp8],   %[ftmp8],           %[ftmp8]            \n\t" \
  "punpcklbh  %[ftmp7],   %[ftmp7],           %[ftmp0]            \n\t" \
  "punpckhbh  %[ftmp8],   %[ftmp8],           %[ftmp0]            \n\t"

#define VP8_MBLOOP_VPSRAB_ADDT                                          \
  "paddh      %[ftmp7],   %[ftmp7],           %[ff_ph_003f]       \n\t" \
  "paddh      %[ftmp8],   %[ftmp8],           %[ff_ph_003f]       \n\t" \
  "psrah      %[ftmp7],   %[ftmp7],           %[ftmp12]           \n\t" \
  "psrah      %[ftmp8],   %[ftmp8],           %[ftmp12]           \n\t" \
  "packsshb   %[ftmp3],   %[ftmp7],           %[ftmp8]            \n\t"

void vp8_mbloop_filter_vertical_edge_mmi(
    unsigned char *src_ptr, int src_pixel_step, const unsigned char *blimit,
    const unsigned char *limit, const unsigned char *thresh, int count) {
  mips_reg tmp[1];
  mips_reg addr[2];
  DECLARE_ALIGNED(8, const uint64_t, srct[1]);
  double ftmp[13];

  __asm__ volatile (
    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_ADDU(%[src_ptr], %[src_ptr], %[tmp0])
    MMI_SUBU(%[src_ptr], %[src_ptr], 0x04)

    "1:                                                             \n\t"
    MMI_SLL (%[tmp0], %[src_pixel_step], 0x01)
    MMI_ADDU(%[addr0], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr0])                          \n\t"
    MMI_ADDU(%[addr0], %[addr0], %[src_pixel_step])
    "gsldlc1    %[ftmp12],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr0])                          \n\t"
    "punpcklbh  %[ftmp1],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp2],   %[ftmp11],          %[ftmp12]           \n\t"

    "gsldlc1    %[ftmp11],  0x07(%[src_ptr])                        \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[src_ptr])                        \n\t"
    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp12],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr0])                          \n\t"
    "punpcklbh  %[ftmp3],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp4],   %[ftmp11],          %[ftmp12]           \n\t"

    "punpcklhw  %[ftmp5],   %[ftmp4],           %[ftmp2]            \n\t"
    "punpckhhw  %[ftmp6],   %[ftmp4],           %[ftmp2]            \n\t"
    "punpcklhw  %[ftmp7],   %[ftmp3],           %[ftmp1]            \n\t"
    "punpckhhw  %[ftmp8],   %[ftmp3],           %[ftmp1]            \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x01)
    MMI_SUBU(%[addr0], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr0])                          \n\t"
    MMI_SUBU(%[addr0], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp12],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr0])                          \n\t"
    "punpcklbh  %[ftmp9],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp10],  %[ftmp11],          %[ftmp12]           \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x02)
    MMI_SUBU(%[addr0], %[src_ptr], %[tmp0])
    "gsldlc1    %[ftmp11],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp11],  0x00(%[addr0])                          \n\t"
    MMI_ADDU(%[addr0], %[addr0], %[src_pixel_step])
    "gsldlc1    %[ftmp12],  0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp12],  0x00(%[addr0])                          \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp11],  %[ftmp11],          %[ftmp12]           \n\t"

    "punpcklhw  %[ftmp1],   %[ftmp11],          %[ftmp10]           \n\t"
    "punpckhhw  %[ftmp2],   %[ftmp11],          %[ftmp10]           \n\t"
    "punpcklhw  %[ftmp3],   %[ftmp0],           %[ftmp9]            \n\t"
    "punpckhhw  %[ftmp4],   %[ftmp0],           %[ftmp9]            \n\t"

    /* ftmp9:q0  ftmp10:q1 */
    "punpcklwd  %[ftmp9],   %[ftmp1],           %[ftmp5]            \n\t"
    "punpckhwd  %[ftmp10],  %[ftmp1],           %[ftmp5]            \n\t"
    /* ftmp11:q2  ftmp12:q3 */
    "punpcklwd  %[ftmp11],  %[ftmp2],           %[ftmp6]            \n\t"
    "punpckhwd  %[ftmp12],  %[ftmp2],           %[ftmp6]            \n\t"
    /* srct[0x00]: q3 */
    "sdc1       %[ftmp12],  0x00(%[srct])                           \n\t"
    /* ftmp1:p3  ftmp2:p2 */
    "punpcklwd  %[ftmp1],   %[ftmp3],           %[ftmp7]            \n\t"
    "punpckhwd  %[ftmp2],   %[ftmp3],           %[ftmp7]            \n\t"
    /* srct[0x08]: p3 */
    "sdc1       %[ftmp1],   0x08(%[srct])                           \n\t"
    /* ftmp5:p1  ftmp6:p0 */
    "punpcklwd  %[ftmp5],   %[ftmp4],           %[ftmp8]            \n\t"
    "punpckhwd  %[ftmp6],   %[ftmp4],           %[ftmp8]            \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[limit])                          \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[limit])                          \n\t"

    /* abs (q3-q2) */
    "pasubub    %[ftmp7],   %[ftmp12],          %[ftmp11]           \n\t"
    "psubusb    %[ftmp0],   %[ftmp7],           %[ftmp8]            \n\t"
    /* abs (q2-q1) */
    "pasubub    %[ftmp7],   %[ftmp11],          %[ftmp10]           \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* ftmp3: abs(q1-q0) */
    "pasubub    %[ftmp3],   %[ftmp10],          %[ftmp9]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp3],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* ftmp4: abs(p1-p0) */
    "pasubub    %[ftmp4],   %[ftmp5],           %[ftmp6]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp4],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* abs (p2-p1) */
    "pasubub    %[ftmp7],   %[ftmp2],           %[ftmp5]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* abs (p3-p2) */
    "pasubub    %[ftmp7],   %[ftmp1],           %[ftmp2]            \n\t"
    "psubusb    %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    /* abs (p0-q0) */
    "pasubub    %[ftmp1],   %[ftmp9],           %[ftmp6]            \n\t"
    "paddusb    %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    /* abs (p1-q1) */
    "pasubub    %[ftmp12],  %[ftmp10],          %[ftmp5]            \n\t"
    "and        %[ftmp12],  %[ftmp12],          %[ff_pb_fe]         \n\t"
    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp8]                                \n\t"
    "psrlh      %[ftmp12],  %[ftmp12],          %[ftmp8]            \n\t"
    "paddusb    %[ftmp12],  %[ftmp1],           %[ftmp12]           \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[blimit])                         \n\t"
    "psubusb    %[ftmp12],  %[ftmp12],          %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp12]           \n\t"
    "xor        %[ftmp12],  %[ftmp12],          %[ftmp12]           \n\t"
    "pcmpeqb    %[ftmp0],   %[ftmp0],           %[ftmp12]           \n\t"

    "gsldlc1    %[ftmp8],   0x07(%[thresh])                         \n\t"
    "gsldrc1    %[ftmp8],   0x00(%[thresh])                         \n\t"
    /* ftmp3: abs(q1-q0)  ftmp4: abs(p1-p0) */
    "psubusb    %[ftmp4],   %[ftmp4],           %[ftmp8]            \n\t"
    "psubusb    %[ftmp3],   %[ftmp3],           %[ftmp8]            \n\t"
    "or         %[ftmp3],   %[ftmp4],           %[ftmp3]            \n\t"
    "pcmpeqb    %[ftmp3],   %[ftmp3],           %[ftmp12]           \n\t"
    "pcmpeqb    %[ftmp1],   %[ftmp1],           %[ftmp1]            \n\t"
    "xor        %[ftmp1],   %[ftmp3],           %[ftmp1]            \n\t"

    "xor        %[ftmp11],  %[ftmp11],          %[ff_pb_80]         \n\t"
    "xor        %[ftmp10],  %[ftmp10],          %[ff_pb_80]         \n\t"
    "xor        %[ftmp9],   %[ftmp9],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp5],   %[ftmp5],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp2],   %[ftmp2],           %[ff_pb_80]         \n\t"

    "psubsb     %[ftmp3],   %[ftmp5],           %[ftmp10]           \n\t"
    "psubsb     %[ftmp4],   %[ftmp9],           %[ftmp6]            \n\t"
    "paddsb     %[ftmp3],   %[ftmp3],           %[ftmp4]            \n\t"
    "paddsb     %[ftmp3],   %[ftmp3],           %[ftmp4]            \n\t"
    "paddsb     %[ftmp3],   %[ftmp3],           %[ftmp4]            \n\t"
    "and        %[ftmp0],   %[ftmp0],           %[ftmp3]            \n\t"
    "and        %[ftmp3],   %[ftmp1],           %[ftmp0]            \n\t"
    "pandn      %[ftmp0],   %[ftmp1],           %[ftmp0]            \n\t"

    "paddsb     %[ftmp4],   %[ftmp3],           %[ff_pb_04]         \n\t"
    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp12]                               \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ftmp7]            \n\t"
    "xor        %[ftmp8],   %[ftmp8],           %[ftmp8]            \n\t"
    "punpcklbh  %[ftmp7],   %[ftmp7],           %[ftmp4]            \n\t"
    "punpckhbh  %[ftmp8],   %[ftmp8],           %[ftmp4]            \n\t"
    "psrah      %[ftmp7],   %[ftmp7],           %[ftmp12]           \n\t"
    "psrah      %[ftmp8],   %[ftmp8],           %[ftmp12]           \n\t"
    "packsshb   %[ftmp4],   %[ftmp7],           %[ftmp8]            \n\t"
    "psubsb     %[ftmp9],   %[ftmp9],           %[ftmp4]            \n\t"
    "paddsb     %[ftmp3],   %[ftmp3],           %[ff_pb_03]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ftmp7]            \n\t"
    "xor        %[ftmp8],   %[ftmp8],           %[ftmp8]            \n\t"
    "punpcklbh  %[ftmp7],   %[ftmp7],           %[ftmp3]            \n\t"
    "punpckhbh  %[ftmp8],   %[ftmp8],           %[ftmp3]            \n\t"
    "psrah      %[ftmp7],   %[ftmp7],           %[ftmp12]           \n\t"
    "psrah      %[ftmp8],   %[ftmp8],           %[ftmp12]           \n\t"
    "packsshb   %[ftmp3],   %[ftmp7],           %[ftmp8]            \n\t"

    "paddsb     %[ftmp6],   %[ftmp6],           %[ftmp3]            \n\t"

    "li         %[tmp0],    0x07                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp12]                               \n\t"
    VP8_MBLOOP_VPSRAB_ADDH
    "paddh      %[ftmp1],   %[ff_ph_0900],      %[ff_ph_0900]       \n\t"
    "paddh      %[ftmp1],   %[ftmp1],           %[ff_ph_0900]       \n\t"
    "pmulhh     %[ftmp7],   %[ftmp7],           %[ftmp1]            \n\t"
    "pmulhh     %[ftmp8],   %[ftmp8],           %[ftmp1]            \n\t"
    VP8_MBLOOP_VPSRAB_ADDT
    "psubsb     %[ftmp4],   %[ftmp9],           %[ftmp3]            \n\t"
    "xor        %[ftmp9],   %[ftmp4],           %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp4],   %[ftmp6],           %[ftmp3]            \n\t"
    "xor        %[ftmp6],   %[ftmp4],           %[ff_pb_80]         \n\t"

    VP8_MBLOOP_VPSRAB_ADDH
    "paddh      %[ftmp1],   %[ff_ph_0900],      %[ff_ph_0900]       \n\t"
    "pmulhh     %[ftmp7],   %[ftmp7],           %[ftmp1]            \n\t"
    "pmulhh     %[ftmp8],   %[ftmp8],           %[ftmp1]            \n\t"
    VP8_MBLOOP_VPSRAB_ADDT
    "psubsb     %[ftmp4],   %[ftmp10],          %[ftmp3]            \n\t"
    "xor        %[ftmp10],   %[ftmp4],          %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp4],   %[ftmp5],           %[ftmp3]            \n\t"
    "xor        %[ftmp5],   %[ftmp4],           %[ff_pb_80]         \n\t"

    VP8_MBLOOP_VPSRAB_ADDH
    "pmulhh     %[ftmp7],   %[ftmp7],           %[ff_ph_0900]       \n\t"
    "pmulhh     %[ftmp8],   %[ftmp8],           %[ff_ph_0900]       \n\t"
    VP8_MBLOOP_VPSRAB_ADDT
    "psubsb     %[ftmp4],   %[ftmp11],          %[ftmp3]            \n\t"
    "xor        %[ftmp11],  %[ftmp4],           %[ff_pb_80]         \n\t"
    "paddsb     %[ftmp4],   %[ftmp2],           %[ftmp3]            \n\t"
    "xor        %[ftmp2],   %[ftmp4],           %[ff_pb_80]         \n\t"

    "ldc1       %[ftmp12],  0x00(%[srct])                           \n\t"
    "ldc1       %[ftmp8],   0x08(%[srct])                           \n\t"

    "punpcklbh  %[ftmp0],   %[ftmp8],           %[ftmp2]            \n\t"
    "punpckhbh  %[ftmp1],   %[ftmp8],           %[ftmp2]            \n\t"
    "punpcklbh  %[ftmp2],   %[ftmp5],           %[ftmp6]            \n\t"
    "punpckhbh  %[ftmp3],   %[ftmp5],           %[ftmp6]            \n\t"
    "punpcklhw  %[ftmp4],   %[ftmp0],           %[ftmp2]            \n\t"
    "punpckhhw  %[ftmp5],   %[ftmp0],           %[ftmp2]            \n\t"
    "punpcklhw  %[ftmp6],   %[ftmp1],           %[ftmp3]            \n\t"
    "punpckhhw  %[ftmp7],   %[ftmp1],           %[ftmp3]            \n\t"

    "punpcklbh  %[ftmp0],   %[ftmp9],           %[ftmp10]           \n\t"
    "punpckhbh  %[ftmp1],   %[ftmp9],           %[ftmp10]           \n\t"
    "punpcklbh  %[ftmp2],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpckhbh  %[ftmp3],   %[ftmp11],          %[ftmp12]           \n\t"
    "punpcklhw  %[ftmp8],   %[ftmp0],           %[ftmp2]            \n\t"
    "punpckhhw  %[ftmp9],   %[ftmp0],           %[ftmp2]            \n\t"
    "punpcklhw  %[ftmp10],  %[ftmp1],           %[ftmp3]            \n\t"
    "punpckhhw  %[ftmp11],  %[ftmp1],           %[ftmp3]            \n\t"

    "punpcklwd  %[ftmp0],   %[ftmp6],           %[ftmp10]           \n\t"
    "punpckhwd  %[ftmp1],   %[ftmp6],           %[ftmp10]           \n\t"

    "gssdlc1    %[ftmp0],   0x07(%[src_ptr])                        \n\t"
    "gssdrc1    %[ftmp0],   0x00(%[src_ptr])                        \n\t"
    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])
    "gssdlc1    %[ftmp1],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp1],   0x00(%[addr0])                          \n\t"

    "punpcklwd  %[ftmp0],   %[ftmp7],           %[ftmp11]           \n\t"
    "punpckhwd  %[ftmp1],   %[ftmp7],           %[ftmp11]           \n\t"
    MMI_ADDU(%[addr0], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp0],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp0],   0x00(%[addr0])                          \n\t"
    MMI_ADDU(%[addr0], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp1],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp1],   0x00(%[addr0])                          \n\t"

    "punpcklwd  %[ftmp1],   %[ftmp5],           %[ftmp9]            \n\t"
    "punpckhwd  %[ftmp0],   %[ftmp5],           %[ftmp9]            \n\t"
    MMI_SUBU(%[addr0], %[src_ptr], %[src_pixel_step])
    "gssdlc1    %[ftmp0],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp0],   0x00(%[addr0])                          \n\t"
    MMI_SUBU(%[addr0], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp1],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp1],   0x00(%[addr0])                          \n\t"

    "punpcklwd  %[ftmp1],   %[ftmp4],           %[ftmp8]            \n\t"
    "punpckhwd  %[ftmp0],   %[ftmp4],           %[ftmp8]            \n\t"
    MMI_SUBU(%[addr0], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp0],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp0],   0x00(%[addr0])                          \n\t"
    MMI_SUBU(%[addr0], %[addr0], %[src_pixel_step])
    "gssdlc1    %[ftmp1],   0x07(%[addr0])                          \n\t"
    "gssdrc1    %[ftmp1],   0x00(%[addr0])                          \n\t"
    "addiu      %[count],   %[count],           -0x01               \n\t"

    MMI_SLL(%[tmp0], %[src_pixel_step], 0x03)
    MMI_ADDU(%[src_ptr], %[src_ptr], %[tmp0])
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [ftmp10]"=&f"(ftmp[10]),            [ftmp11]"=&f"(ftmp[11]),
      [ftmp12]"=&f"(ftmp[12]),            [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [limit]"r"(limit),                [blimit]"r"(blimit),
      [srct]"r"(srct),                  [thresh]"r"(thresh),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [ff_ph_003f]"f"(ff_ph_003f),      [ff_ph_0900]"f"(ff_ph_0900),
      [ff_pb_03]"f"(ff_pb_03),          [ff_pb_04]"f"(ff_pb_04),
      [ff_pb_80]"f"(ff_pb_80),          [ff_pb_fe]"f"(ff_pb_fe)
    : "memory"
  );
}

#define VP8_SIMPLE_HPSRAB                                               \
  "psllh      %[ftmp0],   %[ftmp5],           %[ftmp8]            \n\t" \
  "psrah      %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t" \
  "psrlh      %[ftmp0],   %[ftmp0],           %[ftmp8]            \n\t" \
  "psrah      %[ftmp1],   %[ftmp5],           %[ftmp10]           \n\t" \
  "psllh      %[ftmp1],   %[ftmp1],           %[ftmp8]            \n\t" \
  "or         %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

void vp8_loop_filter_simple_horizontal_edge_mmi(unsigned char *src_ptr,
                                                int src_pixel_step,
                                                const unsigned char *blimit) {
  uint32_t tmp[1], count = 2;
  mips_reg addr[2];
  double ftmp[12];

  __asm__ volatile (
    "li         %[tmp0],    0x08                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp8]                                \n\t"
    "li         %[tmp0],    0x03                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp10]                               \n\t"
    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp11]                               \n\t"

    "1:                                                             \n\t"
    "gsldlc1    %[ftmp3],   0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp3],   0x00(%[blimit])                         \n\t"

    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gsldlc1    %[ftmp2],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp2],   0x00(%[addr1])                          \n\t"
    "gsldlc1    %[ftmp7],   0x07(%[addr0])                          \n\t"
    "gsldrc1    %[ftmp7],   0x00(%[addr0])                          \n\t"
    "pasubub    %[ftmp1],   %[ftmp7],           %[ftmp2]            \n\t"
    "and        %[ftmp1],   %[ftmp1],           %[ff_pb_fe]         \n\t"
    "psrlh      %[ftmp1],   %[ftmp1],           %[ftmp11]           \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsldlc1    %[ftmp6],   0x07(%[addr1])                          \n\t"
    "gsldrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"
    "gsldlc1    %[ftmp0],   0x07(%[src_ptr])                        \n\t"
    "gsldrc1    %[ftmp0],   0x00(%[src_ptr])                        \n\t"
    "pasubub    %[ftmp5],   %[ftmp6],           %[ftmp0]            \n\t"
    "paddusb    %[ftmp5],   %[ftmp5],           %[ftmp5]            \n\t"
    "paddusb    %[ftmp5],   %[ftmp5],           %[ftmp1]            \n\t"
    "psubusb    %[ftmp5],   %[ftmp5],           %[ftmp3]            \n\t"
    "xor        %[ftmp3],   %[ftmp3],           %[ftmp3]            \n\t"
    "pcmpeqb    %[ftmp5],   %[ftmp5],           %[ftmp3]            \n\t"

    "xor        %[ftmp2],   %[ftmp2],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ff_pb_80]         \n\t"
    "psubsb     %[ftmp2],   %[ftmp2],           %[ftmp7]            \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp3],   %[ftmp0],           %[ff_pb_80]         \n\t"
    "psubsb     %[ftmp0],   %[ftmp3],           %[ftmp6]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"
    "paddsb     %[ftmp2],   %[ftmp2],           %[ftmp0]            \n\t"
    "and        %[ftmp5],   %[ftmp5],           %[ftmp2]            \n\t"

    "paddsb     %[ftmp5],   %[ftmp5],           %[ff_pb_04]         \n\t"
    VP8_SIMPLE_HPSRAB
    "psubsb     %[ftmp3],   %[ftmp3],           %[ftmp0]            \n\t"
    "xor        %[ftmp3],   %[ftmp3],           %[ff_pb_80]         \n\t"
    "gssdlc1    %[ftmp3],   0x07(%[src_ptr])                        \n\t"
    "gssdrc1    %[ftmp3],   0x00(%[src_ptr])                        \n\t"

    "psubsb     %[ftmp5],   %[ftmp5],           %[ff_pb_01]         \n\t"
    VP8_SIMPLE_HPSRAB
    "paddsb     %[ftmp6],   %[ftmp6],           %[ftmp0]            \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gssdlc1    %[ftmp6],   0x07(%[addr1])                          \n\t"
    "gssdrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"

    "addiu      %[count],   %[count],           -0x01               \n\t"
    MMI_ADDIU(%[src_ptr], %[src_ptr], 0x08)
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [ftmp10]"=&f"(ftmp[10]),            [ftmp11]"=&f"(ftmp[11]),
      [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),            [addr1]"=&r"(addr[1]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [blimit]"r"(blimit),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [src_pixel_step_x2]"r"((mips_reg)(src_pixel_step<<1)),
      [ff_pb_fe]"f"(ff_pb_fe),          [ff_pb_80]"f"(ff_pb_80),
      [ff_pb_04]"f"(ff_pb_04),          [ff_pb_01]"f"(ff_pb_01)
    : "memory"
  );
}

void vp8_loop_filter_simple_vertical_edge_mmi(unsigned char *src_ptr,
                                              int src_pixel_step,
                                              const unsigned char *blimit) {
  uint32_t tmp[1], count = 2;
  mips_reg addr[2];
  DECLARE_ALIGNED(8, const uint64_t, srct[1]);
  double ftmp[12];

  __asm__ volatile (
    "li         %[tmp0],    0x08                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp8]                                \n\t"
    "li         %[tmp0],    0x20                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp10]                               \n\t"

    MMI_ADDU(%[src_ptr], %[src_ptr], %[src_pixel_step_x4])
    MMI_SUBU(%[src_ptr], %[src_ptr], 0x02)

    "1:                                                             \n\t"
    MMI_ADDU(%[addr0], %[src_ptr], %[src_pixel_step])
    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step_x2])
    "gslwlc1    %[ftmp0],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp0],   0x00(%[addr1])                          \n\t"
    MMI_ADDU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gslwlc1    %[ftmp6],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp6],   %[ftmp6],           %[ftmp0]            \n\t"

    MMI_ADDU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gslwlc1    %[ftmp0],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp0],   0x00(%[addr1])                          \n\t"
    "gslwlc1    %[ftmp4],   0x03(%[src_ptr])                        \n\t"
    "gslwrc1    %[ftmp4],   0x00(%[src_ptr])                        \n\t"

    "punpcklbh  %[ftmp4],   %[ftmp4],           %[ftmp0]            \n\t"
    "punpckhhw  %[ftmp5],   %[ftmp4],           %[ftmp6]            \n\t"
    "punpcklhw  %[ftmp4],   %[ftmp4],           %[ftmp6]            \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gslwlc1    %[ftmp7],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp7],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gslwlc1    %[ftmp6],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp6],   %[ftmp6],           %[ftmp7]            \n\t"

    MMI_SUBU(%[addr1], %[addr0], %[src_pixel_step_x4])
    "gslwlc1    %[ftmp1],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp1],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x4])
    "gslwlc1    %[ftmp0],   0x03(%[addr1])                          \n\t"
    "gslwrc1    %[ftmp0],   0x00(%[addr1])                          \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp0],           %[ftmp1]            \n\t"

    "punpckhhw  %[ftmp2],   %[ftmp0],           %[ftmp6]            \n\t"
    "punpcklhw  %[ftmp0],   %[ftmp0],           %[ftmp6]            \n\t"
    "punpckhwd  %[ftmp1],   %[ftmp0],           %[ftmp4]            \n\t"
    "punpcklwd  %[ftmp0],   %[ftmp0],           %[ftmp4]            \n\t"
    "punpckhwd  %[ftmp3],   %[ftmp2],           %[ftmp5]            \n\t"
    "punpcklwd  %[ftmp2],   %[ftmp2],           %[ftmp5]            \n\t"

    "li         %[tmp0],    0x01                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "pasubub    %[ftmp6],   %[ftmp3],           %[ftmp0]            \n\t"
    "and        %[ftmp6],   %[ftmp6],           %[ff_pb_fe]         \n\t"
    "psrlh      %[ftmp6],   %[ftmp6],           %[ftmp9]            \n\t"
    "pasubub    %[ftmp5],   %[ftmp1],           %[ftmp2]            \n\t"
    "paddusb    %[ftmp5],   %[ftmp5],           %[ftmp5]            \n\t"
    "paddusb    %[ftmp5],   %[ftmp5],           %[ftmp6]            \n\t"

    "gsldlc1    %[ftmp7],   0x07(%[blimit])                         \n\t"
    "gsldrc1    %[ftmp7],   0x00(%[blimit])                         \n\t"
    "psubusb    %[ftmp5],   %[ftmp5],           %[ftmp7]            \n\t"
    "xor        %[ftmp7],   %[ftmp7],           %[ftmp7]            \n\t"
    "pcmpeqb    %[ftmp5],   %[ftmp5],           %[ftmp7]            \n\t"

    "sdc1       %[ftmp0],   0x00(%[srct])                           \n\t"
    "sdc1       %[ftmp3],   0x08(%[srct])                           \n\t"

    "xor        %[ftmp0],   %[ftmp0],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp3],   %[ftmp3],           %[ff_pb_80]         \n\t"
    "psubsb     %[ftmp0],   %[ftmp0],           %[ftmp3]            \n\t"

    "xor        %[ftmp6],   %[ftmp1],           %[ff_pb_80]         \n\t"
    "xor        %[ftmp3],   %[ftmp2],           %[ff_pb_80]         \n\t"
    "psubsb     %[ftmp7],   %[ftmp3],           %[ftmp6]            \n\t"
    "paddsb     %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    "paddsb     %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    "paddsb     %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    "and        %[ftmp5],   %[ftmp5],           %[ftmp0]            \n\t"
    "paddsb     %[ftmp5],   %[ftmp5],           %[ff_pb_04]         \n\t"

    "li         %[tmp0],    0x03                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "psllh      %[ftmp0],   %[ftmp5],           %[ftmp8]            \n\t"
    "psrah      %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"
    "psrlh      %[ftmp0],   %[ftmp0],           %[ftmp8]            \n\t"

    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "psrah      %[ftmp7],   %[ftmp5],           %[ftmp9]            \n\t"
    "psllh      %[ftmp7],   %[ftmp7],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp7]            \n\t"
    "psubsb     %[ftmp3],   %[ftmp3],           %[ftmp0]            \n\t"
    "xor        %[ftmp3],   %[ftmp3],           %[ff_pb_80]         \n\t"
    "psubsb     %[ftmp5],   %[ftmp5],           %[ff_pb_01]         \n\t"

    "li         %[tmp0],    0x03                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "psllh      %[ftmp0],   %[ftmp5],           %[ftmp8]            \n\t"
    "psrah      %[ftmp0],   %[ftmp0],           %[ftmp9]            \n\t"
    "psrlh      %[ftmp0],   %[ftmp0],           %[ftmp8]            \n\t"

    "li         %[tmp0],    0x0b                                    \n\t"
    "mtc1       %[tmp0],    %[ftmp9]                                \n\t"
    "psrah      %[ftmp5],   %[ftmp5],           %[ftmp9]            \n\t"
    "psllh      %[ftmp5],   %[ftmp5],           %[ftmp8]            \n\t"
    "or         %[ftmp0],   %[ftmp0],           %[ftmp5]            \n\t"
    "paddsb     %[ftmp6],   %[ftmp6],           %[ftmp0]            \n\t"
    "xor        %[ftmp6],   %[ftmp6],           %[ff_pb_80]         \n\t"

    "ldc1       %[ftmp0],   0x00(%[srct])                           \n\t"
    "ldc1       %[ftmp4],   0x08(%[srct])                           \n\t"

    "punpckhbh  %[ftmp1],   %[ftmp0],           %[ftmp6]            \n\t"
    "punpcklbh  %[ftmp0],   %[ftmp0],           %[ftmp6]            \n\t"
    "punpcklbh  %[ftmp2],   %[ftmp3],           %[ftmp4]            \n\t"
    "punpckhbh  %[ftmp3],   %[ftmp3],           %[ftmp4]            \n\t"

    "punpckhhw  %[ftmp6],   %[ftmp0],           %[ftmp2]            \n\t"
    "punpcklhw  %[ftmp0],   %[ftmp0],           %[ftmp2]            \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x4])
    "gsswlc1    %[ftmp0],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp0],   0x00(%[addr1])                          \n\t"
    "punpckhhw  %[ftmp5],   %[ftmp1],           %[ftmp3]            \n\t"
    "punpcklhw  %[ftmp1],   %[ftmp1],           %[ftmp3]            \n\t"

    "dsrl       %[ftmp0],   %[ftmp0],           %[ftmp10]           \n\t"
    MMI_SUBU(%[addr1], %[addr0], %[src_pixel_step_x4])
    "gsswlc1    %[ftmp0],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp0],   0x00(%[addr1])                          \n\t"
    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gsswlc1    %[ftmp6],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"

    "dsrl       %[ftmp6],   %[ftmp6],           %[ftmp10]           \n\t"
    "gsswlc1    %[ftmp1],   0x03(%[src_ptr])                        \n\t"
    "gsswrc1    %[ftmp1],   0x00(%[src_ptr])                        \n\t"

    MMI_SUBU(%[addr1], %[src_ptr], %[src_pixel_step])
    "gsswlc1    %[ftmp6],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp6],   0x00(%[addr1])                          \n\t"

    MMI_ADDU(%[addr1], %[src_ptr], %[src_pixel_step_x2])
    "gsswlc1    %[ftmp5],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"

    "dsrl       %[ftmp1],   %[ftmp1],           %[ftmp10]           \n\t"
    "gsswlc1    %[ftmp1],   0x03(%[addr0])                          \n\t"
    "gsswrc1    %[ftmp1],   0x00(%[addr0])                          \n\t"

    "dsrl       %[ftmp5],   %[ftmp5],           %[ftmp10]           \n\t"
    MMI_ADDU(%[addr1], %[addr0], %[src_pixel_step_x2])
    "gsswlc1    %[ftmp5],   0x03(%[addr1])                          \n\t"
    "gsswrc1    %[ftmp5],   0x00(%[addr1])                          \n\t"

    MMI_ADDU(%[src_ptr], %[src_ptr], %[src_pixel_step_x8])
    "addiu      %[count],   %[count],           -0x01               \n\t"
    "bnez       %[count],   1b                                      \n\t"
    : [ftmp0]"=&f"(ftmp[0]),              [ftmp1]"=&f"(ftmp[1]),
      [ftmp2]"=&f"(ftmp[2]),              [ftmp3]"=&f"(ftmp[3]),
      [ftmp4]"=&f"(ftmp[4]),              [ftmp5]"=&f"(ftmp[5]),
      [ftmp6]"=&f"(ftmp[6]),              [ftmp7]"=&f"(ftmp[7]),
      [ftmp8]"=&f"(ftmp[8]),              [ftmp9]"=&f"(ftmp[9]),
      [ftmp10]"=&f"(ftmp[10]),            [ftmp11]"=&f"(ftmp[11]),
      [tmp0]"=&r"(tmp[0]),
      [addr0]"=&r"(addr[0]),            [addr1]"=&r"(addr[1]),
      [src_ptr]"+&r"(src_ptr),          [count]"+&r"(count)
    : [blimit]"r"(blimit),              [srct]"r"(srct),
      [src_pixel_step]"r"((mips_reg)src_pixel_step),
      [src_pixel_step_x2]"r"((mips_reg)(src_pixel_step<<1)),
      [src_pixel_step_x4]"r"((mips_reg)(src_pixel_step<<2)),
      [src_pixel_step_x8]"r"((mips_reg)(src_pixel_step<<3)),
      [ff_pb_fe]"f"(ff_pb_fe),          [ff_pb_80]"f"(ff_pb_80),
      [ff_pb_04]"f"(ff_pb_04),          [ff_pb_01]"f"(ff_pb_01)
    : "memory"
  );
}

/* Horizontal MB filtering */
void vp8_loop_filter_mbh_mmi(unsigned char *y_ptr, unsigned char *u_ptr,
                             unsigned char *v_ptr, int y_stride, int uv_stride,
                             loop_filter_info *lfi) {
  vp8_mbloop_filter_horizontal_edge_mmi(y_ptr, y_stride, lfi->mblim, lfi->lim,
                                        lfi->hev_thr, 2);

  if (u_ptr)
    vp8_mbloop_filter_horizontal_edge_mmi(u_ptr, uv_stride, lfi->mblim,
                                          lfi->lim, lfi->hev_thr, 1);

  if (v_ptr)
    vp8_mbloop_filter_horizontal_edge_mmi(v_ptr, uv_stride, lfi->mblim,
                                          lfi->lim, lfi->hev_thr, 1);
}

/* Vertical MB Filtering */
void vp8_loop_filter_mbv_mmi(unsigned char *y_ptr, unsigned char *u_ptr,
                             unsigned char *v_ptr, int y_stride, int uv_stride,
                             loop_filter_info *lfi) {
  vp8_mbloop_filter_vertical_edge_mmi(y_ptr, y_stride, lfi->mblim, lfi->lim,
                                      lfi->hev_thr, 2);

  if (u_ptr)
    vp8_mbloop_filter_vertical_edge_mmi(u_ptr, uv_stride, lfi->mblim, lfi->lim,
                                        lfi->hev_thr, 1);

  if (v_ptr)
    vp8_mbloop_filter_vertical_edge_mmi(v_ptr, uv_stride, lfi->mblim, lfi->lim,
                                        lfi->hev_thr, 1);
}

/* Horizontal B Filtering */
void vp8_loop_filter_bh_mmi(unsigned char *y_ptr, unsigned char *u_ptr,
                            unsigned char *v_ptr, int y_stride, int uv_stride,
                            loop_filter_info *lfi) {
  vp8_loop_filter_horizontal_edge_mmi(y_ptr + 4 * y_stride, y_stride, lfi->blim,
                                      lfi->lim, lfi->hev_thr, 2);
  vp8_loop_filter_horizontal_edge_mmi(y_ptr + 8 * y_stride, y_stride, lfi->blim,
                                      lfi->lim, lfi->hev_thr, 2);
  vp8_loop_filter_horizontal_edge_mmi(y_ptr + 12 * y_stride, y_stride,
                                      lfi->blim, lfi->lim, lfi->hev_thr, 2);

  if (u_ptr)
    vp8_loop_filter_horizontal_edge_mmi(u_ptr + 4 * uv_stride, uv_stride,
                                        lfi->blim, lfi->lim, lfi->hev_thr, 1);

  if (v_ptr)
    vp8_loop_filter_horizontal_edge_mmi(v_ptr + 4 * uv_stride, uv_stride,
                                        lfi->blim, lfi->lim, lfi->hev_thr, 1);
}

/* Vertical B Filtering */
void vp8_loop_filter_bv_mmi(unsigned char *y_ptr, unsigned char *u_ptr,
                            unsigned char *v_ptr, int y_stride, int uv_stride,
                            loop_filter_info *lfi) {
  vp8_loop_filter_vertical_edge_mmi(y_ptr + 4, y_stride, lfi->blim, lfi->lim,
                                    lfi->hev_thr, 2);
  vp8_loop_filter_vertical_edge_mmi(y_ptr + 8, y_stride, lfi->blim, lfi->lim,
                                    lfi->hev_thr, 2);
  vp8_loop_filter_vertical_edge_mmi(y_ptr + 12, y_stride, lfi->blim, lfi->lim,
                                    lfi->hev_thr, 2);

  if (u_ptr)
    vp8_loop_filter_vertical_edge_mmi(u_ptr + 4, uv_stride, lfi->blim, lfi->lim,
                                      lfi->hev_thr, 1);

  if (v_ptr)
    vp8_loop_filter_vertical_edge_mmi(v_ptr + 4, uv_stride, lfi->blim, lfi->lim,
                                      lfi->hev_thr, 1);
}

void vp8_loop_filter_bhs_mmi(unsigned char *y_ptr, int y_stride,
                             const unsigned char *blimit) {
  vp8_loop_filter_simple_horizontal_edge_mmi(y_ptr + 4 * y_stride, y_stride,
                                             blimit);
  vp8_loop_filter_simple_horizontal_edge_mmi(y_ptr + 8 * y_stride, y_stride,
                                             blimit);
  vp8_loop_filter_simple_horizontal_edge_mmi(y_ptr + 12 * y_stride, y_stride,
                                             blimit);
}

void vp8_loop_filter_bvs_mmi(unsigned char *y_ptr, int y_stride,
                             const unsigned char *blimit) {
  vp8_loop_filter_simple_vertical_edge_mmi(y_ptr + 4, y_stride, blimit);
  vp8_loop_filter_simple_vertical_edge_mmi(y_ptr + 8, y_stride, blimit);
  vp8_loop_filter_simple_vertical_edge_mmi(y_ptr + 12, y_stride, blimit);
}
