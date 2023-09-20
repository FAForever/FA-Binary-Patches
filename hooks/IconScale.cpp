#include "../define.h"
asm(
    ".section h0; .set h0,0x85DE05;"
    "jmp "QU(ScaleIcons)";"
    "nop;"
    "nop;"
);