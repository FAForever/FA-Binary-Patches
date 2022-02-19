//HOOK selectionPriority ROffset = 0x3D1AE9

#include <stdlib.h>
#include "../preprocessor/define.h"
#include "../preprocessor/macro.h"

__asm__
(
    ".equ by_pass_address,"QU(minimapMesh)"-0x007D1AE9 \n"
);

__asm__ volatile
(
    "jmp . + by_pass_address \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    ".align 128, 0x0 \n"
);