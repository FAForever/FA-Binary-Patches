#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"


void asm__IsForceReclaim()
{
	asm(
        "mov eax, dword ptr ds:[ebp+0x1C];" // entity
        "mov al, byte ptr ss:[eax+0x126];"  // entity->isForceReclaim
        
        "test al, al;"
        "jz Exit;"
        
        "mov byte ptr ss:[esp+0x13], 0x0;" // energyStorageIsFull = false
        "mov byte ptr ss:[esp+0x12], 0x0;" // massStorageIsFull   = false
        
        "Exit:;"
        "lea edx, ss:[esp+0x94];" //default code
        "jmp 0x0061BC0E;"
	);
}


int ForceReclaim(lua_State *L)
{
    if (lua_gettop(L) != 2)
        L->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(L));
    auto res = GetCScriptObject<Unit>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);

    void *unit = res.object;
    bool flag = lua_toboolean(L, 2);

    GetField<bool>(unit, 8+0x11E) = flag; // bool isForceReclaim

    return 0;
}
using UnitMethodReg = SimRegFuncT<0x00E2D550, 0x00F8D704>;

UnitMethodReg ForceReclaimReg{
    "ForceReclaim",
    "",
    ForceReclaim,
    "Unit"};