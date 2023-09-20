#include "include/moho.h"

bool __thiscall IsLuaFunction(LuaObject *obj)
{
    return obj->Type() == LUA_TFUNCTION;
}