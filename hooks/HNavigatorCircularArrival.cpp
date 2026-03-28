#include "../define.h"
asm(
    ".section h0; .set h0,0x5AE485;"
    "jmp "QU(CircularArrivalCheck)";"
    ".byte 0x90;"
);
