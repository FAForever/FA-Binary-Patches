#include "include/moho.h"

int *__thiscall luaplus_assert(int *_this, const char *Source) asm("0x457880");

void __stdcall _CxxThrowException(void *pExceptionObject, void *pThrowInfo) asm("0x00A89950");

bool __thiscall IsLuaFunction(LuaObject *obj)
{
    // if (!obj->m_state)
    // {
    //     int exception_obj[10];
    //     luaplus_assert(exception_obj, "m_state");
    //     _CxxThrowException(exception_obj, (void *)0xEC23F0);
    // }
    return obj->Type() == LUA_TFUNCTION;
}