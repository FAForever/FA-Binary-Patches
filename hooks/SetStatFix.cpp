#include "../define.h"
asm(
  ".section h0; .set h0,0x6CCA2A;"
  "jmp "QU(SetStatCheck)";"
  "nop;"
  "nop;"
);
