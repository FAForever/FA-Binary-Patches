#include "global.h"
#include "moho.h"
#include "CObject.h"
#include "magic_classes.h"

// Per-weapon engine fire lock: the Lua-callable UnitWeapon:SetWeaponBusy.
//
// UnitWeapon::CanFire consults the UNIT-level busy flag, so one weapon's
// salvo locks every sibling of the unit (busy tax) and a sibling's
// SetBusy(false) unlocks a mid-salvo weapon into the dropped-dispatch
// window. The Lua salvo states set this per-weapon byte instead, and
// CanFire reads it (hooks/PerWeaponBusy.hook). +0xF1 is UnitWeapon pad,
// zero-initialized by the constructor and not serialized.
int SetWeaponBusy(lua_State *L)
{
    if (lua_gettop(L) != 2)
        L->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(L));
    auto res = GetCScriptObject<UnitWeapon>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);
    auto weapon = reinterpret_cast<uintptr_t>(res.object);
    *reinterpret_cast<uint8_t *>(weapon + 0xF1) = lua_toboolean(L, 2);
    return 0;
}

// 0xE2E324 = the engine's own UnitWeapon:SetEnabled register-func slot
// (any method of the class works, see section/ProjectileNewMethods.cpp);
// 0xF8D7A4 = the UnitWeapon class descriptor.
using WeaponMethodReg = SimRegFuncT<0x00E2E324, 0x00F8D7A4>;

WeaponMethodReg WeaponSetBusyReg{
    "SetWeaponBusy",
    "UnitWeapon:SetWeaponBusy(busy)",
    SetWeaponBusy,
    "UnitWeapon"};
