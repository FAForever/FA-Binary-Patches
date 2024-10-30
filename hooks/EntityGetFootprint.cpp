#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"

asm(
// Moho::Entity::Entity
SECTION(0, 0x00677BC9)
    "mov dword ptr [esi+0x1F8], ebx;" // 0x1F9 bool forceAltFootprint

//  Moho::Entity::GetFootprint
SECTION(1, 0x006788A5)
    "jmp "QU(asm__GetFootprint)";"
    "nop;"
    "nop;"
);