
#include "include/LuaAPI.h"
#define NON_GENERAL_REG(var_) [var_] "g"(var_)

//PatcherList_LCreateTable_e_lua_createtable
void e_lua_createtable(lua_State *l, int narr, int nhash)
{ // copied from lua_newtable
    asm("mov     esi, %[l];" // lua_State
        "mov     eax, [esi+0x10];"
        "mov     ecx, [eax+0x2C];"
        "cmp     ecx, [eax+0x24];"
        "jb      short loc__90D130;"
        "cmp     dword ptr [eax+0x28], 0;"
        "jnz     short loc__90D130;"
        "push    esi;"
        "call    0x915D90;"
        "add     esp, 4;"
        "loc__90D130: ;"
        "mov     edi, [esi+8];"
        "push    %[nhash];"
        "push    %[narr];"
        "push    esi;"
        "call    0x00927320;" // luaH_new
        "movzx   edx, byte ptr [eax+4];"
        "mov     [edi], edx;"
        "mov     [edi+4], eax;"
        "mov     eax, [esi+8];"
        "mov     ecx, [esi+0x14];"
        "add     esp, 0x0C;"
        "cmp     eax, [ecx+4];"
        "jb      short loc__90D173;"
        "mov     edx, [esi+0x18];"
        "sub     edx, eax;"
        "mov     edi, 8;"
        "cmp     edx, edi;"
        "jg      short loc__90D16D;"
        "push    1;"
        "push    esi;"
        "call    0x913990;"
        "add     esp, 8;"
        "loc__90D16D:;"
        "add     [esi+8], edi;"
        "jmp RETURN;"
        "loc__90D173:;"
        "add     dword ptr [esi+8], 8;"
        "RETURN:"
        :
        : NON_GENERAL_REG(l),
          NON_GENERAL_REG(narr),
          NON_GENERAL_REG(nhash)
        : "edx", "ecx", "eax", "edi", "esi");
}