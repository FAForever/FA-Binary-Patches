#include "../define.h"

asm(
    ".section h0; .set h0,0x0086F754;"
    "CALL " QU(LoadReclaimCameraDistance) ";"
    "NOP;"
);
