// ==========================================================================
// NavigatorCircularArrival.cpp — Circular build range arrival check
//
// Hook 2 of 2: In CAiPathNavigator::UpdateCurrentPosition (0x5AE2D0),
// before the normal cell-equality arrival check, test if the unit is
// within the circular build range (tagged in mGoal.mPos2 by Hook 1).
// If within range, signal "arrived" — the unit stops and building starts.
// ==========================================================================

#include "moho.h"
#include "global.h"
#include "MovementConfig.h"

// Called from the asm hook to do the circular distance check
extern "C" int __cdecl CheckCircularArrival(uint8_t* pathNav)
{
    int rangeCells = *(int*)(pathNav + OFF_PATHNAV_TAG_RANGE);
    if (rangeCells <= 0) return 0;  // not a build move

    int buildX = *(int*)(pathNav + OFF_PATHNAV_TAG_X);
    int buildZ = *(int*)(pathNav + OFF_PATHNAV_TAG_Z);

    // Validate tag matches the active goal. If the goal changed (e.g. new order
    // interrupted the build move without SetGoal zeroing mPos2), clear stale tag.
    int goalX = *(int*)(pathNav + OFF_PATHNAV_GOAL_X);
    int goalZ = *(int*)(pathNav + OFF_PATHNAV_GOAL_Z);
    if (buildX != goalX || buildZ != goalZ) {
        *(int*)(pathNav + OFF_PATHNAV_TAG_RANGE) = 0;
        return 0;
    }

    uint32_t curPos = *(uint32_t*)(pathNav + OFF_PATHNAV_CURPOS);
    int curX = (int)(short)(curPos & 0xFFFF);
    int curZ = (int)(short)(curPos >> 16);

    int dx = curX - buildX;
    int dz = curZ - buildZ;
    int distSq = dx * dx + dz * dz;
    int rangeSq = rangeCells * rangeCells;

    if (distSq <= rangeSq) {
        *(int*)(pathNav + OFF_PATHNAV_TAG_RANGE) = 0;  // clear tag (one-shot)
        return 1;  // signal arrival
    }
    return 0;
}

// Top-level asm hook at 0x5AE485
// EBX = CAiPathNavigator*
// Replaces 6 bytes: mov ecx,[ebx+1C]; mov eax,[ebx+24]

asm(
    ".global _CircularArrivalCheck\n"
    "_CircularArrivalCheck:\n"

    // EBX (CAiPathNavigator*) and EBP are callee-saved by cdecl ABI.
    // EAX/ECX/EDX are scratch — we set them ourselves below or the
    // overwritten instructions in .Lca_not_build will overwrite them.
    "push ebx\n"
    "call _CheckCircularArrival\n"
    "add esp, 4\n"
    "test eax, eax\n"
    "jz .Lca_not_build\n"

    // IN RANGE: signal arrival
    "xor ecx, ecx\n"
    "mov dword ptr [ebx+0x0C], ecx\n"
    "mov eax, dword ptr [ebx+0x24]\n"
    "mov dword ptr [ebx+0x28], eax\n"
    "mov dword ptr [ebx+0x64], ecx\n"
    "pop edi\n"
    "pop esi\n"
    "mov esp, ebp\n"
    "pop ebp\n"
    "ret 4\n"

    ".Lca_not_build:\n"
    "mov ecx, dword ptr [ebx+0x1C]\n"
    "mov eax, dword ptr [ebx+0x24]\n"
    "jmp 0x5AE48B\n"
);
