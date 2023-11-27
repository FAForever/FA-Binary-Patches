#include "../define.h"
asm(
    ".section h0; .set h0,0x4D0F02;"
    "mov dword ptr [0x10A8FE4], " QU(VDist3) ";");
