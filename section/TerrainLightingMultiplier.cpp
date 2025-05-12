#include "moho.h"
#define NON_GENERAL_REG(var_) [var_] "g"(var_)

float TerrainLighMult = 0;


void GetTerrainLighMult()
{
    asm(
        "cmp %[TerrainLighMult], 0x0;"
        "je Default;"
        
        "fld %[TerrainLighMult];"
        "jmp 0x00801647;"
        
        "Default:;"
        "fld dword ptr ds:[ecx+0x2D8];"
        "jmp 0x00801647;"

        :
        : [TerrainLighMult] "m" (TerrainLighMult)
        :
    );
}

int LuaSetTerrainLightingMultiplier(lua_State *l)
{
    float mult = luaL_optnumber(l, 1, 0);
    
    TerrainLighMult = mult;

    return 0;
}

UIRegFunc SetTerrainLightingMultiplierReg{"SetTerrainLightingMultiplier", "SetTerrainLightingMultiplier(mult:float)", LuaSetTerrainLightingMultiplier};