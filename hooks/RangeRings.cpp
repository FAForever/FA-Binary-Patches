//HOOK RangeRings ROffset = 0x3F944D

#include <stdlib.h>
#include "../preprocessor/define.h"
#include "../preprocessor/macro.h"

__asm__
(
    ".equ by_pass_address,"QU(RangeRings)"-0x007F944D \n"
);

__asm__ volatile
(
    "call . + by_pass_address \n"
	".byte 0xE8 \n"
    ".align 128, 0x0 \n"
);
