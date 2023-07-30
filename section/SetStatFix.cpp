#include "include/moho.h"

void PushTrue()
{
    register lua_State *l asm("eax");
    lua_pushboolean(l, true);
}

void SetStatCheck()
{
    asm(
        // mov    ebx, eax //pointer to our stat in other thread
        "mov     eax, [edi];" // pointer to lua state
        "cmp ebx, 0;"
        "je SetStatCheck_invalid;" // go to return 1
        "mov     esi, 3;"
        "jmp 0x6CCA31;" // get back into normal function flow
        "SetStatCheck_invalid:"
        "call %[PushTrue];"
        "mov eax, 1;"
        "pop     edi;"
        "pop     esi;"
        "pop     ebx;"
        "mov     esp, ebp;"
        "pop     ebp;"
        "ret;"
        :
        : [PushTrue] "i"(PushTrue)
        :
        );
}
