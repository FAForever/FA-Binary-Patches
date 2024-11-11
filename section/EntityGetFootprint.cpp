#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"

int ForceAltFootprint(lua_State *L)
{

    if (lua_gettop(L) != 2)
        L->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(L));
    auto res = GetCScriptObject<Unit>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);

    void *unit = res.object;
    bool flag = lua_toboolean(L, 2);

    GetField<bool>(unit, 8+0x1F9) = flag; // bool forceAltFootprint

    return 0;
}
using UnitMethodReg = SimRegFuncT<0x00E2D550, 0x00F8D704>;

UnitMethodReg UseAltFootprintReg{
    "ForceAltFootprint",
    "",
    ForceAltFootprint,
    "Unit"};


void asm__GetFootprint()
{
    asm(
        "cmp   byte ptr [ecx+0x11C], 0;" //  this->sstiData.usingAltFp
        "jnz   0x006788AE;"
        "cmp   byte ptr [ecx+0x1F9], 0;" // bool forceAltFootprint
        "jnz   0x006788AE;"
        "jmp   0x006788C2;"
    );
}