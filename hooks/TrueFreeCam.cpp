#include "../define.h"
asm(
  //HOOK ClampFocusPos
  ".section h0; .set h0,0x007A9470;"
  "JMP "QU(TrueFreeCamDetour)";"
  "NOP;"
);
