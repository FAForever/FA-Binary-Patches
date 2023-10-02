#include "../define.h"
asm(
    ".section h0; .set h0,0x8D1A85;"
    "jmp "QU(StoreFPS)";"
    "nop;"
);
