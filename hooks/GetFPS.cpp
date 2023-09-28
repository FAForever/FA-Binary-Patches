#include "../define.h"
asm(
    ".section h0; .set h0,0x8D1A7E;"
    "jmp "QU(StoreFPS)";"
);
