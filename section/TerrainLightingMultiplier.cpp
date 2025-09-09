#include "moho.h"
#include "magic_classes.h"

float TerrainLighMult = 0;
ConDescReg terrainLightingMultiplier_var{"ui_TerrainLightingMultiplier", "", &TerrainLighMult};

void GetTerrainLighMult()
{
    asm(
        "cmp %[TerrainLighMult], 0x0;"
        "je Default;"
        
        "fld %[TerrainLighMult];"
        "jmp 0x00801647;"
        
        "Default:;"
        // Default code
        "mov     eax, [edx+0x0A4];"
        "jmp 0x00801645;"
        :
        : [TerrainLighMult] "m" (TerrainLighMult)
        :
    );
}
