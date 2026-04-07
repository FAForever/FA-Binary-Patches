// ==========================================================================
// EngineerBuildRange.cpp — Tag navigator for circular build range arrival
//
// Hook 1 of 2: At 0x5F7754 (inside the move-issuing if-block in TaskTick),
// we replace the push 0;push 0;push 0 + NewMoveTask call sequence.
// We execute NewMoveTask ourselves, then tag pathNavigator.mGoal.mPos2
// with the build center + MaxBuildDistance.
//
// Must tag AFTER NewMoveTask because SetGoal zeros mPos2 for 1x1 goals.
//
// IMPORTANT: 0x5F776B (++mTaskState) is a SHARED instruction reached by
// both the move and no-move paths. We must NOT overwrite it.
// Our hook at 0x5F7754 (6 bytes) is safely inside the if-block.
// ==========================================================================

#include "moho.h"
#include "global.h"
#include "MovementConfig.h"

// All navigator offsets are in MovementConfig.h

extern "C" void __cdecl TagNavigatorForBuild(uint8_t* task)
{
    uint32_t unitAddr = *(uint32_t*)(task + 0x1C);
    if (!IsValidPtr(unitAddr)) return;

    uint32_t bpAddr = *(uint32_t*)(unitAddr + OFF_UNIT_BLUEPRINT);
    if (!IsValidPtr(bpAddr)) return;
    float maxBD = *(float*)(bpAddr + OFF_BP_MAXBUILDDIST);
    if (maxBD <= 2.0f) return;

    uint32_t navLand = *(uint32_t*)(unitAddr + OFF_UNIT_NAVIGATOR);
    if (!IsValidPtr(navLand)) return;
    uint32_t pathNav = *(uint32_t*)(navLand + OFF_NAVLAND_PATHNAV);
    if (!IsValidPtr(pathNav)) return;

    // ── Tuning ───────────────────────────────────────────────────
    // rangeCells: circular arrival radius in cells (1 cell = 1 world unit)
    //   higher = engineer stops further from center (more time saved)
    //   too high = State 1 check fails on diagonals (build cancelled!)
    //   70% of maxBD: scales with unit type, safe for diagonals
    //     ACU (maxBD=10): rangeCells=7, diagonal=9.9 ✓
    //     T1 eng (maxBD=5): rangeCells=3, diagonal=4.2 ✓
    int rangeCells = (int)(maxBD * 0.89f); //"Martin" 0.9 works fine at max range ,0.9 has a problem in diagonal mex building whitch lead to skipping first mex to bild in a row
    // ────────────────────────────────────────────────────────────
    if (rangeCells < 2) return;

    // Distance check using CELL COORDINATES from the pathNavigator.
    // These are always current (SetGoal just ran), unlike world coords
    // which can be stale for shift-click build queues.
    int goalX = *(int*)(pathNav + OFF_PATHNAV_GOAL_X);
    int goalZ = *(int*)(pathNav + OFF_PATHNAV_GOAL_Z);
    uint32_t curPos = *(uint32_t*)(pathNav + OFF_PATHNAV_CURPOS);
    int curX = (int)(short)(curPos & 0xFFFF);
    int curZ = (int)(short)(curPos >> 16);
    int cdx = goalX - curX;
    int cdz = goalZ - curZ;
    int cellDistSq = cdx * cdx + cdz * cdz;

    // Only tag if the move is long (unit needs to walk far).
    // Short moves (on-site clearing, build queues) use default engine behavior.
    // Unit must be at least rangeCells+3 cells away from goal.
    int tagThreshold = rangeCells + 3;  // = 10 for rangeCells=7
    int tagThresholdSq = tagThreshold * tagThreshold;  // = 100
    if (cellDistSq <= tagThresholdSq) return;

    // Tag pathNavigator with build center (cell coords) + range.
    // Must use cell coords because Hook 2 compares with mCurrentPos (also in cells).
    *(int*)(pathNav + OFF_PATHNAV_TAG_X) = goalX;
    *(int*)(pathNav + OFF_PATHNAV_TAG_Z) = goalZ;
    *(int*)(pathNav + OFF_PATHNAV_TAG_RANGE) = rangeCells;
}

// ==========================================================================
// ASM wrapper — top-level asm, no function prologue.
//
// Hook at 0x5F7754 (the 3x push 0 before NewMoveTask, 6 bytes):
//   Safely inside the if-block (only reached when move is needed).
//   We reproduce the entire NewMoveTask call sequence, then tag navigator.
//
// Original code being replaced/reproduced:
//   0x5F7754: push 0; push 0; push 0          (6 bytes, OVERWRITTEN)
//   0x5F775A: lea edi, [esp+0x10C]            (reproduced in hook)
//   0x5F7761: mov esi, ebp                    (reproduced in hook)
//   0x5F7763: call NewMoveTask                (reproduced in hook)
//   0x5F7768: add esp, 0xC                    (reproduced in hook)
//   → then we tag navigator
//   → then JMP to 0x5F776B (shared ++mTaskState)
// ==========================================================================

asm(
    ".global _EngineerBuildRangeHook\n"
    "_EngineerBuildRangeHook:\n"

    // Reproduce: push 0; push 0; push 0 (args for NewMoveTask)
    "push 0\n"
    "push 0\n"
    "push 0\n"

    // Reproduce: lea edi, [esp+0x10C] (navGoal on stack, adjusted for 3 pushes)
    "lea edi, [esp+0x10C]\n"

    // Reproduce: mov esi, ebp (task pointer)
    "mov esi, ebp\n"

    // Reproduce: call NewMoveTask
    "call 0x6190A0\n"

    // Reproduce: add esp, 0xC (cleanup 3 push args)
    "add esp, 0x0C\n"

    // Now tag the navigator (NewMoveTask + SetGoal already ran)
    "pushad\n"
    "push ebp\n"
    "call _TagNavigatorForBuild\n"
    "add esp, 4\n"
    "popad\n"

    // JMP to shared ++mTaskState (NOT overwritten, safely reached)
    "jmp 0x5F776B\n"
);
