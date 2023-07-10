#include "../define.h"
asm(
  ".section h0; .set h0,0x6CE3B2;"
  "jmp "QU(SimGetCommandQueue)";"
  
  ".section h1; .set h1,0x6CE3B7;"
  "call "QU(SimGetCommandQueueCpp)";"
  "add esp, 0x10;"
);