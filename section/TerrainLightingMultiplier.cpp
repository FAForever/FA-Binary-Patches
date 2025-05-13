#include "moho.h"
#define NON_GENERAL_REG(var_) [var_] "g"(var_)

float TerrainLighMult = 0;

float sunR = 0;
float sunG = 0;
float sunB = 0;

float sunX = 0;
float sunY = 0;
float sunZ = 0;


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

void GetSunColor()
{
    asm(
        //default
        "lea edx, ss:[esp+0x1C];" 
        "push edx;"
        "mov eax, dword ptr ss:[esp];"
        
        "cmp %[sunR], 0x0;"
        "je Default_1;"
        
        "fld dword ptr %[sunR];"
        "fstp dword ptr ds:[eax];"
        "fld dword ptr %[sunG];"
        "fstp dword ptr ds:[eax+0x4];"
        "fld dword ptr %[sunB];"
        "fstp dword ptr ds:[eax+0x8];"
        "add esp, 0x4;"
        "jmp 0x008016E6;"
        
        "Default_1:;"
        "fld dword ptr ds:[ecx+0x2F4];"
        "fstp dword ptr ds:[eax];"
        "fld dword ptr ds:[ecx+0x2F8];"
        "fstp dword ptr ds:[eax+0x4];"
        "fld dword ptr ds:[ecx+0x2FC];"
        "fstp dword ptr ds:[eax+0x8];"
        "add esp, 0x4;"
        "jmp 0x008016E6;"

        :
        : [sunR] "m" (sunR),
          [sunG] "m" (sunG),
          [sunB] "m" (sunB)
        :
    );
}

void GetSunPosition()
{
    asm(
        //default
        "lea eax, ss:[esp+0x2C];" 
        "push eax;"
        "mov eax, dword ptr ss:[esp];"
        
        "cmp %[sunX], 0x0;"
        "je Default_2;"
        
        // "fld dword ptr %[sunX];"
        // "fstp dword ptr ds:[eax];"
        // "fld dword ptr %[sunY];"
        // "fstp dword ptr ds:[eax+0x4];"
        // "fld dword ptr %[sunZ];"
        // "fstp dword ptr ds:[eax+0x8];"
        // "add esp, 0x4;"
        // "jmp 0x00801680;"
        
        "push eax;"
        "mov eax, %[sunX];"
        "mov dword ptr ds:[ecx+0x2DC], eax;"
        "mov eax, %[sunY];"
        "mov dword ptr ds:[ecx+0x2E0], eax;"
        "mov eax, %[sunZ];"
        "mov dword ptr ds:[ecx+0x2E4], eax;"
        "pop eax;"
        
        "Default_2:;"
        "fld dword ptr ds:[ecx+0x2DC];"
        "fstp dword ptr ds:[eax];"
        "fld dword ptr ds:[ecx+0x2E0];"
        "fstp dword ptr ds:[eax+0x4];"
        "fld dword ptr ds:[ecx+0x2E4];"
        "fstp dword ptr ds:[eax+0x8];"
        "add esp, 0x4;"
        "jmp 0x00801680;"

        :
        : [sunX] "m" (sunX),
          [sunY] "m" (sunY),
          [sunZ] "m" (sunZ)
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

int LuaSetSunColorRGB(lua_State *l)
{
    float r = luaL_optnumber(l, 1, 0);
    float g = luaL_optnumber(l, 2, 0);
    float b = luaL_optnumber(l, 3, 0);
    
    if (r > 255 || g > 255 || b > 255)
    {
        sunR = 0;
        sunG = 0;
        sunB = 0;
        return 0;
    }
    
    sunR = r / 100;
    sunG = g / 100;
    sunB = b / 100;

    return 0;
}

UIRegFunc SetSunColorRGBReg{"SetSunColorRGB", "SetSunColorRGB(R:float, G:float, B:float)", LuaSetSunColorRGB};

int LuaSetSunPositionXYZ(lua_State *l)
{
    float x = luaL_optnumber(l, 1, 0);
    float y = luaL_optnumber(l, 2, 0);
    float z = luaL_optnumber(l, 3, 0);
    
    if (x == 0 & y == 0 & z == 0)
    {
        sunX = 0;
        sunY = 0;
        sunZ = 0;
        return 0;
    }
    
    sunX = x / 100;
    sunY = y / 100;
    sunZ = z / 100;

    return 0;
}

UIRegFunc SetSunPositionXYZReg{"SetSunPositionXYZ", "SetSunPositionXYZ(X:float, Y:float, Z:float)", LuaSetSunPositionXYZ};