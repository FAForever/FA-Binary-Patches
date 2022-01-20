//HOOK TeamColorMode ROffset = 0x447E51

#include <stdlib.h>
#include "../preprocessor/define.h"
#include "../preprocessor/macro.h"

__asm__
(
    ".equ by_pass_address,"QU(TeamColorMode)"-0x00847E51 \n"
);

__asm__ volatile
(
    "jmp . + by_pass_address \n"
    "nop \n"
    "nop \n"
    "nop \n"
    ".align 128, 0x0 \n"
);