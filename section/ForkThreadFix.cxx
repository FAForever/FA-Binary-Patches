#include "LuaAPI.h"

SHARED bool __thiscall IsLuaFunction(LuaObject *obj)
{
    return obj->Type() == LUA_TFUNCTION;
}