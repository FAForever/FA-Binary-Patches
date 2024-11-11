#include "../define.h"
asm(
  ".section h1; .set h1,0x45A926;"
  "push eax;"
  "call "QU(InitCtors)";"
  "pop eax;"
  "ret 8;"
);