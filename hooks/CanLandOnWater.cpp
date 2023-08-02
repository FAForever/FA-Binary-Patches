#include "../define.h"
asm(
    // checks terrain height and gets foot points in that function
    ".section h0; .set h0,0x0062B84A;"
    "push " QU(CANLANDONWATER) ";");
