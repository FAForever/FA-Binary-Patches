#include "../define.h"
asm(
  ".section h0; .set h0,0x61FF7B;"
  "JMP "QU(StopReclaim)";"
  "nop;"
  "nop;"
  
  ".section h1; .set h1,0x620071;"
  "JMP "QU(RestoreReclaimValues)";"
  "nop;"
  "nop;"
);