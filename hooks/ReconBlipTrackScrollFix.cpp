#include "../define.h"

// Moho::ReconBlip::BuildSnapshot + 0x1C1
// Replaces: sub esi, 0xD8
asm(
    ".section h0; .set h0,0x005BF171;"
    "JMP " QU(ReconBlipTrackScrollFix) ";"
    "NOP;"
);
