#include "../define.h"
asm(".section h0; .set h0,0x00430C0E;"
    "call " QU(OnWindowMessage) ";");
