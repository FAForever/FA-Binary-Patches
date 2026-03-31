#include "../define.h"
asm(
  // === FIRST Cast loop (fill rings) ===
  // Reduce batch size from 1000 to 30 for stencil early-out optimization
  ".section h0; .set h0,0x7EF77E;"
  ".byte 0x1E, 0x00, 0x00, 0x00;"

  ".section h1; .set h1,0x7EF785;"
  ".byte 0x1E, 0x00, 0x00, 0x00;"

  // Hook after func_Draw_Rings: intermediate RangeMask flush
  // Replaces 7-byte mov eax,[esp+0x84] with call + 2 NOPs
  ".section h2; .set h2,0x7EF7EA;"
  "call " QU(IntermediateRangeMask) ";"
  "nop; nop;"
);
