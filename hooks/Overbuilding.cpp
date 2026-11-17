#include "../define.h"

// USERUNIT_CanBeBuiltAt has just tested whether a visible static/dead unit's
// skirt overlaps the proposed building skirt. Replace the vanilla immediate
// rejection with a check for a queued reclaim command targeting that blocker.
asm(
    ".section h0; .set h0,0x008C1A4F;"
    "JMP " QU(OverbuildingPlacementHook) ";"
    "NOP;"
    "NOP;"
    "NOP;"
);
