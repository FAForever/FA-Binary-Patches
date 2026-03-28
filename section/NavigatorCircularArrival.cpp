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

// Called from the asm hook to do the circular distance check
extern "C" int __cdecl CheckCircularArrival(uint8_t* pathNav)
{
    int rangeCells = *(int*)(pathNav + 0x48);  // mGoal.mPos2.x1 (our tag)
    if (rangeCells <= 0) return 0;  // not a build move

    int buildX = *(int*)(pathNav + 0x40);      // mGoal.mPos2.x0
    int buildZ = *(int*)(pathNav + 0x44);      // mGoal.mPos2.z0

    // Current position: HPathCell at +0x24, packed as x:int16 low, z:int16 high
    uint32_t curPos = *(uint32_t*)(pathNav + 0x24);
    int curX = (int)(short)(curPos & 0xFFFF);
    int curZ = (int)(short)(curPos >> 16);

    int dx = curX - buildX;
    int dz = curZ - buildZ;
    int distSq = dx * dx + dz * dz;
    int rangeSq = rangeCells * rangeCells;

    if (distSq <= rangeSq) {
        *(int*)(pathNav + 0x48) = 0;  // clear tag (one-shot)
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

    "pushad\n"
    "push ebx\n"
    "call _CheckCircularArrival\n"
    "add esp, 4\n"
    "test eax, eax\n"
    "popad\n"
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
