#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"

// Any changes made here WILL NOT affect units without ForceReverseMove flag
// Without this flag all movement calculations go through default route

void asm__UnitForceReverseCheck()
{
	asm(
        "movss xmm2, dword ptr ss:[esp+0x8C];" //default code
        
        "cmp byte ptr ss:[ebx+0x127], 0x1;"   //unit.forceReverseMove
        
        "jne 0x005B3504;"   // no ForceReverseMove, use default logic
     
        "jmp 0x005B354A;"   // otherwise jump straight to "reverse" case
	);
}


// This called every few ticks (5-10?) until command is done
void asm__BackUpDistanceCheck()
{
	asm(
        "movss xmm0, dword ptr ds:[ecx+0x70];" //default code
        
        "cmp byte ptr ss:[ebx+0x127], 0x1;"   //unit.forceReverseMove
        
        "jne 0x005B357D;"                     //no ForceReverseMove, use default route to compare BackUpDistance

        "jmp 0x005B3591;"                     //ignore all checks and go to "reverse"            
	);
}

// When unit is in ForceReverseMove mode, movementTypeID should always be 5
// When moving in formation using reverse move, some units randomly change it to normal move (id = 7)
// and then after a few tick return to reverse again. So we have to check it here and set to 5 manually
// for all units in ForceReverseMove mode. 
void asm__MovementTypeIDCheck()
{
	asm(
        "cmp byte ptr ss:[ebx+0x127], 0x1;"  //unit.forceReverseMove
        "jne Exit;"                          //no ForceReverseMove, no need to change ID
        
        "mov eax, 0x5;"
        "mov dword ptr ss:[esp+0x64], eax;"
        
        "Exit:;"   
        "mov eax, dword ptr ss:[esp+0x64];"    // default code
        "cmp eax, 5;"                          // 
        "jmp 0x005B356D;"                            
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

    GetField<bool>(unit, 8+0x11F) = flag; // bool forceReverseMove

    return 0;
}
using UnitMethodReg = SimRegFuncT<0x00E2D550, 0x00F8D704>;

UnitMethodReg ForceReverseMoveReg{
    "ForceReverseMove",
    "",
    ForceReverseMove,
    "Unit"};