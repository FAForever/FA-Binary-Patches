//HOOK TeamColorModeRenderer ROffset = 0x45DB68

#include <stdlib.h>
#include "../preprocessor/define.h"
#include "../preprocessor/macro.h"

__asm__
(
    ".equ by_pass_address,"QU(TeamColorModeRenderer)"-0x0085DB68 \n"
);

__asm__ volatile
(
    "jmp . + by_pass_address \n"
    "nop \n"
    "nop \n"
    ".align 128, 0x0 \n"
);