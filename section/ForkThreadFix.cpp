#include "include/moho.h"

void IsLuaFunction()
{
    asm(
        "sub     esp, 0x28;"
        "cmp     dword ptr [ecx+8], 0;"
        "jnz     loc_907836;"
        "push    offset 0xD44C30; "
        "lea     ecx, [esp+0x2C-0x28];"
        "call    0x457880;"// luaplus_assert
        "push    0xEC23F0 ;"
        "lea     eax, [esp+0x2C-0x28];"
        "push    eax             ; "
        "call    0xA89950 ;"//__CxxThrowException
        "loc_907836: ;"
        "mov     ecx, [ecx+0x0C];"
        "xor     eax, eax;"
        "cmp     ecx, 7;" // check for lua function
        "setz    al;"
        "add     esp, 0x28;"
        "ret;");
}