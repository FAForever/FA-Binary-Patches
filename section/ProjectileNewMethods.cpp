#include "global.h"
#include "moho.h"
#include "CObject.h"
#include "magic_classes.h"

int GetMaxZigZag(lua_State *L) {
    auto res = GetCScriptObject<Projectile>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);
    auto projectile = reinterpret_cast<uintptr_t>(res.object);
    lua_pushnumber(L, *reinterpret_cast<float*>(projectile + 0x36C));
    return 1;
}

int GetZigZagFrequency(lua_State *L) {
    auto res = GetCScriptObject<Projectile>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);
    auto projectile = reinterpret_cast<uintptr_t>(res.object);
    lua_pushnumber(L, *reinterpret_cast<float*>(projectile + 0x370));
    return 1;
}

int GetCurrentTargetPositionXYZ(lua_State *L) {
    auto res = GetCScriptObject<Projectile>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);
    auto projectile = reinterpret_cast<uintptr_t>(res.object);
    volatile Vector3f pos{};
    if (*reinterpret_cast<uintptr_t*>(projectile + 0x2EC)) {
        asm("call 0x5E2A90"
            :
            :"b"(0), "c"(projectile + 0x2EC), "a"(&pos)
            :);
    }
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);
    return 3;
}

int SetNewTargetGroundXYZ(lua_State *L) {
    if (lua_gettop(L) != 4)
        L->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 4, lua_gettop(L));
    auto res = GetCScriptObject<Projectile>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);

    volatile struct target_details {
        uint32_t mode;
        void *unit; //-0xC
        uint32_t dword8;
        float x, y, z;
        int dword18;
        char byte1C;
    } details;

    details.x = lua_tonumber(L, 2);
    details.y = lua_tonumber(L, 3);
    details.z = lua_tonumber(L, 4);

    details.mode = 2;
    details.unit = 0;
    details.dword8 = 0;
    details.dword18= -1;
    details.byte1C = 0;

    auto projectile = reinterpret_cast<uintptr_t>(res.object);
    asm("call 0x5D5670"
        :
        :"a"(projectile + 0x2EC), "S"(&details)
        :);
    return 0;
}

using ProjectileMethodReg = SimRegFuncT<0xE2A0DC, 0xF8D784>;

ProjectileMethodReg ProjectileGetMaxZigZag{
    "GetMaxZigZag",
    "zigzag = Projectile:GetMaxZigZag()",
    GetMaxZigZag,
    "Projectile"};

ProjectileMethodReg ProjectileGetZigZagFrequency{
    "GetZigZagFrequency",
    "frequency = Projectile:GetZigZagFrequency()",
    GetZigZagFrequency,
    "Projectile"};

ProjectileMethodReg ProjectileGetCurrentTargetPositionXYZ{
    "GetCurrentTargetPositionXYZ",
    "x, y, z = Projectile:GetCurrentTargetPositionXYZ",
    GetCurrentTargetPositionXYZ,
    "Projectile"};

ProjectileMethodReg ProjectileSetNewTargetGroundXYZ{
    "SetNewTargetGroundXYZ",
    "Projectile:SetNewTargetGroundXYZ(x, y, z)",
    SetNewTargetGroundXYZ,
    "Projectile"};
