#include "../define.h"

asm(
  ".section h0; .set h0,0x00915A11;"
  "jmp "QU(GCFix)";"
  "nop;"  
);
