#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"


void UnitForceReverseCheck()
{
	asm(
        "movss xmm2, dword ptr ss:[esp+0x8C];" //default
        
        "cmp byte ptr ss:[ebx+0x687], 0x1;"   //bool unit.forceReverseMove
        
        "jne 0x005B3504;"

        //jump straight to "reverse" case if there is forceReverse flag
        "jmp 0x005B354A;"
	);
}


//This called every few ticks (5-10?) until command is done
void BackUpDistanceCheck()
{
	asm(
        "movss xmm0, dword ptr ds:[ecx+0x70];" //default
        
        "cmp byte ptr ss:[ebx+0x687], 0x1;"   //bool forceReverseMove
        
        "jne 0x005B357D;"                     //default route to compare BackUpDistance

        "jmp 0x005B3591;"                     //ignore all checks and go to "reverse"            
	);
}


void UnitInit()
{
	asm(
        "mov byte ptr ss:[ebp+0x687], bl;" //new bool unit.forceReverseMove
        
        "mov byte ptr ss:[ebp+0x688], bl;" //default
        
        "jmp 0x006A5704;"
	);
}


int ForceReverseMove(lua_State *L)
{

    if (lua_gettop(L) != 2)
        L->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(L));
    auto res = GetCScriptObject<Unit>(L, 1);
    if (res.IsFail())
        L->LuaState->Error("%s", res.reason);

    void *unit = res.object;
    bool flag = lua_toboolean(L, 2);

    GetField<bool>(unit, 0x687) = flag; // bool forceReverseMove

    return 0;
}
using UnitMethodReg = SimRegFuncT<0x00E2D550, 0x00F8D704>;

UnitMethodReg ForceReverseMoveReg{
    "ForceReverseMove",
    "",
    ForceReverseMove,
    "Unit"};