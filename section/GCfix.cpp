#include "include/CObject.h"
#include "include/magic_classes.h"
#include "include/moho.h"
#include "include/utility.h"
#include <cmath>

int __cdecl luaB_collectgarbage(lua_State*) asm("0x0090F240");
int __cdecl luaB_gcinfo  (lua_State*) asm("0x0090F200");

UIRegFunc CollectGarbageReg{"collectgarbage", "", luaB_collectgarbage};
UIRegFunc GCInfoReg{"gcinfo", "", luaB_gcinfo};

void GCFix()
{
    asm(
        "mov     cl, [eax+5];"
        "movzx   edx, cl   ;"
        "and edx, -7;"
        "jmp 0x00915A17;"
    );
}