
#include "include/CObject.h"
#include "include/moho.h"

typedef unsigned char lu_byte;
struct Table
{
    int pad0;
    int pad1;
    lu_byte tt;
    lu_byte lsizenode; /* log2 of size of `node' array */
    lu_byte marked;    /* 1<<p means tagmethod(p) is not present */
    lu_byte flags;
    struct Table *metatable;
    TObject *array; /* array part */
    int pad2;
    int pad3;
    int pad4;
    int sizearray; /* size of `array' array */
};
VALIDATE_SIZE(Table, 0x24)

#define NON_GENERAL_REG(var_) [var_] "g"(var_)
void lua_createtable(lua_State *l, int narr, int nhash = 0)
{
    asm(                     // copied from lua_newtable
        "mov     esi, %[l];" // lua_State
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

Vector3f ToVector(lua_State *l, int index)
{
    Vector3f res{0, 0, 0};
    if (!lua_istable(l, index))
    {
        DebugLog("Not vector");
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
    lua_createtable(l, 3);
    lua_pushnumber(l, v.x);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, v.y);
    lua_rawseti(l, -2, 2);
    lua_pushnumber(l, v.z);
    lua_rawseti(l, -2, 3);
}

void PushVector(lua_State *l, Vector2f v)
{
    lua_createtable(l, 2);
    lua_pushnumber(l, v.x);
    lua_rawseti(l, -2, 1);
    lua_pushnumber(l, v.z);
    lua_rawseti(l, -2, 2);
}

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

    Table *t = (Table *)lua_topointer(l, index);
    lua_createtable(l, t->sizearray, t->lsizenode); // result table
    lua_pushvalue(l, index);                        // input vectors
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

// PatcherList_UIFuncRegs_UWorldViewProjectMultiple
luaFuncDescReg UWorldViewProjectMultiple = {0x00E491E8,
                                            "ProjectMultiple",
                                            "CUIWorldView",
                                            "WorldView:ProjectMultiple(vectors)",
                                            0x00000000,
                                            ProjectMultiple,
                                            0x00F8D88C};
