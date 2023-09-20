#pragma once

#include "moho.h"
#include "LuaAPI.h"

Vector3f ToVector(lua_State *l, int index)
{
    Vector3f res{0, 0, 0};
    if (!lua_istable(l, index))
    {
        return res;
    }
    lua_pushvalue(l, index);
    lua_rawgeti(l, -1, 1);
    res.x = lua_tonumber(l, -1);
    lua_rawgeti(l, -2, 2);
    res.y = lua_tonumber(l, -1);
    lua_rawgeti(l, -3, 3);
    res.z = lua_tonumber(l, -1);
    lua_pop(l, 4);
    return res;
}

void PushVector(lua_State *l, Vector3f v)
{
    lua_createtable(l, 3, 0);
    lua_pushnumber(l, v.x);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, v.y);
    lua_rawseti(l, -2, 2);
    lua_pushnumber(l, v.z);
    lua_rawseti(l, -2, 3);
}

void PushVector(lua_State *l, Vector2f v)
{
    lua_createtable(l, 2, 0);
    lua_pushnumber(l, v.x);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, v.z);
    lua_rawseti(l, -2, 2);
}