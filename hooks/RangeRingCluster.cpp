#include "../define.h"

// Replace the 5-byte `mov eax, ds:0x10a6438` (sWldMap) at 0x007EF5E2 with a
// 5-byte JMP into our trampoline. The trampoline calls a C compaction
// routine that drops range rings whose XY positions are within
// ui_RangeRingClusterDistance of an already-kept ring this frame, then
// re-executes the displaced instruction and jumps back to 0x007EF5E7.
asm(
    ".section h0; .set h0,0x007EF5E2;"
    "jmp " QU(RangeRingClusterTrampoline) ";"
);
