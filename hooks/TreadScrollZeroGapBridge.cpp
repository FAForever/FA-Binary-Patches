#include "../define.h"

// Replaces the straight-line UserEntity -> WorldMesh tread copy block at
// 0x008B9284 and resumes after it at 0x008B92C4.
asm(
    ".section h5; .set h5,0x008B9284;"
    "JMP " QU(TreadScrollZeroGapBridge) ";"
);
