#include "include/LuaAPI.h"
#include "include/global.h"
#include "include/utility.h"

int VDist3(lua_State *l)
{
    if (lua_gettop(l) != 2)
    {
        luaL_error(l, s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(l));
        return 0;
    }
    Vector3f a = ToVector(l, 1);
    Vector3f b = ToVector(l, 2);
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    float dist = sqrtf(dx * dx + dy * dy + dz * dz);
    lua_pushnumber(l, dist);
    return 1;
}