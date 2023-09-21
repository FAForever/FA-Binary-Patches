#include "../define.h"
asm(
    ".section h0; .set h0,0x85DE05;"
    "jmp "QU(ScaleIcons)";"
    "nop;"
    "nop;" 
    
    ".section h1; .set h1,0x85DF02;"
    "jmp "QU(ScalePauseIcon)";"
    "nop;"
    "nop;"
    
    ".section h2; .set h2,0x85E003;"
    "jmp "QU(ScaleStunIcon)";"
    "nop;"
    
    ".section h3; .set h3,0x85DCF7;"
    "jmp "QU(ScaleUnderlay)";"
    "nop;"
    "nop;"
);
