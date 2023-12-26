#include "include/CObject.h"
#include "include/magic_classes.h"
#include "include/moho.h"
#include "include/utility.h"

void Project(float *camera, const Vector3f *v, Vector2f *result)
{
    asm(
        "call 0x471080;"
        :
        : "a"(result),
          "d"(v),
          "c"(camera)
        //: "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
    );
}

Vector2f ProjectVec(const Vector3f &v, float *camera)
{
    Vector2f res;
    Project(camera, &v, &res);
    return res;
}

void ProjectVectors(lua_State *l, int index, float *camera)
{
    const char *t = (const char *)lua_topointer(l, index);
    uint32_t asize;
    uint8_t hbits;
    GetTableAH(t, &asize, &hbits);
    lua_createtable(l, asize, hbits); // result table
    lua_pushvalue(l, index);          // input vectors
    lua_pushnil(l);
    while (lua_next(l, -2)) // -1 = value, -2 =  key, -3 = table, -4 = result table
    {
        Vector3f v = ToVector(l, -1);
        Vector2f p = ProjectVec(v, camera);
        lua_pushvalue(l, -2); // key
        PushVector(l, p);     // value
        lua_rawset(l, -6);
        lua_pop(l, 1);
    }

    lua_pop(l, 1);
}

int ProjectMultiple(lua_State *l)
{
    if (lua_gettop(l) != 2)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(l));
    }
    if (!lua_istable(l, 2))
    {
        luaL_argerror(l, 2, "table expected.");
    }
    Result<CUIWorldView> r = GetCScriptObject<CUIWorldView>(l, 1);
    if (r.IsFail())
    {
        lua_pushstring(l, r.reason);
        lua_error(l);
        return 0;
    }
    void *worldview = r.object;
    if (worldview == nullptr)
        return 0;
    void *camera = (void *)(*(int(__thiscall **)(int))(*(int *)((int)worldview + 284) + 12))((int)worldview + 284);
    if (camera == nullptr)
        return 0;
    float *geomcamera = (float *)(*(int(__thiscall **)(void *))(*(int *)camera + 8))(camera);
    ProjectVectors(l, 2, geomcamera);
    return 1;
}

// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft:GetScreenPos(GetSelectedUnits()[1]))
// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft.ProjectMultiple(import("/lua/maui/text.lua").Text(GetFrame(0)),{{1,2,3}}))
// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft:ProjectMultiple({{1,2,3}}))
// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft:ProjectMultiple({GetSelectedUnits()[1]:GetPosition()}))
// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft.ProjectMultiple())
// UI_Lua reprsl(import("/lua/ui/game/worldview.lua").viewLeft.ProjectMultiple({},{}))

using WorldViewMethodReg = UIRegFunc<0x00E491E8, 0x00F8D88C>;

WorldViewMethodReg WorldViewProjectMultiple{
    "ProjectMultiple",
    "WorldView:ProjectMultiple(vectors)",
    ProjectMultiple,
    "CUIWorldView"};

int SetCustomRender(lua_State *l)
{
    if (lua_gettop(l) != 2)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(l));
    }
    Result<CUIWorldView> r = GetCScriptObject<CUIWorldView>(l, 1);
    if (r.IsFail())
    {
        lua_pushstring(l, r.reason);
        lua_error(l);
        return 0;
    }
    void *worldview = r.object;
    if (worldview == nullptr)
        return 0;

    bool state = lua_toboolean(l, 2);
    *(char *)((int)worldview + 377) = state;
    return 0;
}

WorldViewMethodReg WorldViewSetCustomRender{
    "SetCustomRender",
    "WorldView:SetCustomRender(bool)",
    SetCustomRender,
    "CUIWorldView"};