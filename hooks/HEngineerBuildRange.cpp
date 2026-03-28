#include "../define.h"
asm(
    ".section h0; .set h0,0x5F7754;"
    "jmp "QU(EngineerBuildRangeHook)";"
    ".byte 0x90;"
);
