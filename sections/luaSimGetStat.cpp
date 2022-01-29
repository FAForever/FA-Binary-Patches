#include <stdlib.h>

char SalemWaterOnlyBp[1536];
char SalemDefaultBp[4];

// This patch allows to turn off Salem's amphibious ability to exclude accidental land moves
// We take default Salem's blueprint and copy it to `SalemWaterOnlyBp`
// Then change 2 values in it: MinWaterDepth = 1.5 and OccupancyCaps = 8
// Only these 2 do matter in movement calculations, other bp values stay the same
// Then we just change a pointer to blueprint in unit object depending on what mode should be activated
// Amphibious mode ON: SalemDefaultBp, amphibious mode OFF: SalemWaterOnlyBp.

void luaSimGetStat()
{
	__asm__
	(
        "cmp byte ptr [eax], 0x68 \n"
        "jne EXIT \n "
        "cmp byte ptr [eax+0x1], 0x31 \n "
        "jne EXIT \n "
        "push eax \n"
        "push ebx \n"
        "mov eax, %[SalemDefaultBp] \n"
        "cmp eax, 0x0 \n"
        "je UpdateBlueprints \n "
        "cmp byte ptr [eax + 0xDA], 0x9 \n"         // Check if SalemDefaultBp pointer is still valid (OccupancyCaps = 9 means it is)
        "mov eax, %[SalemWaterOnlyBp] \n"           // and if not then update both blueprints (prevents crashes on simulation restart)           
        "je dummyExists \n "
        "UpdateBlueprints: \n"
        "mov eax, %[SalemWaterOnlyBp] \n"
        "push ebx \n"
        "push ecx \n"
        "push edx \n"
        "mov ebx, dword ptr [ebx+0x74] \n"
        "mov %[SalemDefaultBp], ebx \n "
        "mov eax, ebx \n "
        "mov ebx, %[SalemWaterOnlyBp] \n "
        "mov edx, dword ptr [eax] \n "
        "mov [ebx], edx \n "
        "mov ecx, 0x4 \n"
        "MemCpyCycle: \n"
        "add ebx, 4 \n"
        "mov edx, dword ptr [eax+ecx] \n "
        "mov [ebx], edx \n "
        "add ecx, 4 \n"
        "cmp ecx, 1532 \n"
        "jbe MemCpyCycle \n "
        "mov dword ptr [ebx - 0x51C], 0x3fc00000 \n"  //MinWaterDepth = 1.5 
        "mov dword ptr [ebx - 0x522], 0x8 \n"         //OccupancyCaps = 8
        "pop edx \n"
        "pop ecx \n"
        "pop ebx \n"
        "dummyExists: \n"
        "cmp dword ptr [esp+0x20], 0x0 \n"
        "jne SetAmphON \n "
        "mov eax, dword ptr [ebx+0x74] \n "
        "cmp byte ptr [eax + 0xDA], 0x8 \n"
        "je POPEXIT \n"
        "mov eax, %[SalemWaterOnlyBp] \n"
        "mov dword ptr [ebx+0x74], eax \n"
        "jmp POPEXIT \n"
        "SetAmphON: \n"
        "mov eax, dword ptr [ebx+0x74] \n "
        "cmp byte ptr [eax + 0xDA], 0x9 \n"
        "je POPEXIT \n"
        "mov eax, %[SalemDefaultBp] \n"
        "mov dword ptr [ebx+0x74], eax \n"
        "POPEXIT: \n"
        "pop ebx \n"
        "pop eax \n"
        "EXIT: \n"
        "mov esi, eax \n"
        "add esp,0x8 \n "
        "test esi, esi \n "
        "jmp 0x006CC760 \n "
        :
        : [SalemWaterOnlyBp] "i" (SalemWaterOnlyBp), [SalemDefaultBp] "m" (SalemDefaultBp)
        :
	);
}

/* Lua sim function: unit:GetStat(char statName, int defValue)
unit:GetStat("h1_SetSalemAmph", 1)

if (statName.at(0) == "h") {
    if (statName.at(1) == "1") {

        if !SalemWaterOnlyBp {
            SalemWaterOnlyBp = defaultBP                            //handmade memcpy here :)
            SalemWaterOnlyBp.Footprint.MinWaterDepth = 1.5          //default 0
            SalemWaterOnlyBp.Footprint.OccupancyCaps = 8            //default 9  Only these 2 parameters do matter
        }

        if (defValue == 0){                                         //turn off amphibious
            if (unit.Blueprint.Footprint.OccupancyCaps != 8) {      
                unit.Blueprint = SalemWaterOnlyBp                      
            }
        } else {                                                    //turn on                                   
            if (unit.Blueprint.Footprint.OccupancyCaps != 9) {      
                unit.Blueprint = SalemDefaultBp
            }  
        }
    }
} */