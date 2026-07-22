#include "../define.h"

// Keep the deterministic phase rebase separate from the complete WorldMesh
// copy-block hook. TreadScrollZeroGapBridge owns all four mapped output stores,
// so there is only one writer for the left/right previous/current pairs.
asm(
    ".section h0; .set h0,0x008B9033;"
    "JMP " QU(TreadScrollPhaseRebase) ";"
    "NOP;"
    "NOP;"
    "NOP;"
    "NOP;"
    "NOP;"
);
