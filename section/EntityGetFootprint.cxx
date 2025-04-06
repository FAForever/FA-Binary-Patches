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

    GetField<bool>(unit, 8 + 0x11D) = flag; // bool forceAltFootprint

    return 0;
}
using UnitMethodReg = SimRegFuncT<0x00E2D550, 0x00F8D704>;

UnitMethodReg UseAltFootprintReg{
    "ForceAltFootprint",
    "",
    ForceAltFootprint,
    "Unit"};

SHARED void *__thiscall Moho__Entity__GetFootprint(Entity *entity)
{
    void *bp = GetField<void *>(entity, 0x6C);
    if (!bp)
    {
        runtime_error err{string{"Attempt to get footprint on nameless entity"}};
        _CXXThrowException(&err, (void *)0xEC210C);
    }
    void *sstiData = Offset<void *>(entity, 0x78);
    bool usingAltFp = GetField<bool>(sstiData, 0xA4);
    bool forceAltFp = GetField<bool>(sstiData, 0xA5);
    if (usingAltFp || forceAltFp)
    {
        return Offset<void *>(bp, 0xE8);
    }
    else
    {
        return Offset<void *>(bp, 0xD8);
    }
}