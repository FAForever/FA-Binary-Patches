#include "include/moho.h"
#include "include/CObject.h"

void *CheckUserUnit(LuaObject *obj, LuaState *ls)
{
    void *result;
    asm(
        "call 0x00822B80;"
        : "=a"(result)
        : [obj] "a"(obj), [ls] "D"(ls)
        :);

    return result;
}

namespace Moho
{
    namespace UserUnit
    {
        inline float GetFractionComplete(void *unit)
        {
            return *((float *)unit + 44);
        }

        inline void *GetMeshInstance(void *unit)
        {
            return *((void **)unit + 11);
        }

        inline void *GetUserUnit(LuaObject *obj, LuaState *luaState)
        {
            return CheckUserUnit(obj, luaState);
        }
    } // namespace UserUnit

    namespace MeshInstance
    {
        inline void UpdateInterpolatedTransform(void *mesh)
        {
            reinterpret_cast<void(__stdcall *)(void *)>(0x007DEC80)(mesh);
        }
    } // namespace MeshInstance

}

int GetInterpolatedPosition(lua_State *l)
{
    if (lua_gettop(l) != 1)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 1, lua_gettop(l));
    }
    Result<UserUnit> r = GetCScriptObject<UserUnit>(l, 1);
    if (r.IsFail())
    {
        lua_pushstring(l, r.reason);
        lua_error(l);
        return 0;
    }
    void *unit = r.object;
    if (unit == nullptr)
        return 0;
    float *mesh = (float *)Moho::UserUnit::GetMeshInstance(unit);
    if (mesh == nullptr)
        return 0;
    Moho::MeshInstance::UpdateInterpolatedTransform(mesh);
    lua_pushnumber(l, mesh[34]);
    lua_pushnumber(l, mesh[35]);
    lua_pushnumber(l, mesh[36]);
    return 3;
}
// for testing
// UI_Lua LOG(GetSelectedUnits()[1].GetInterpolatedPosition)
// UI_Lua LOG(GetSelectedUnits()[1].GetInterpolatedPosition())
// UI_Lua LOG(GetSelectedUnits()[1]:GetInterpolatedPosition())
// UI_Lua LOG(GetSelectedUnits()[1].GetInterpolatedPosition{})
// UI_Lua LOG(GetSelectedUnits()[1].GetInterpolatedPosition(GetCamera"WorldCamera"))

int GetFractionComplete(lua_State *l)
{
    if (lua_gettop(l) != 1)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 1, lua_gettop(l));
    }

    Result<UserUnit> r = GetCScriptObject<UserUnit>(l, 1);

    if (r.IsFail())
    {
        lua_pushstring(l, r.reason);
        lua_error(l);
        return 0;
    }
    void *unit = r.object;
    if (unit == nullptr)
        return 0;
    lua_pushnumber(l, Moho::UserUnit::GetFractionComplete(unit));
    return 1;
}
// UI_Lua LOG(GetSelectedUnits()[1].GetFractionComplete)
// UI_Lua LOG(GetSelectedUnits()[1].GetFractionComplete())
// UI_Lua LOG(GetSelectedUnits()[1]:GetFractionComplete())
// UI_Lua LOG(GetSelectedUnits()[1].GetFractionComplete{})

// PatcherList_UIFuncRegs_UUserUnitGetInterpolatedPosition
luaFuncDescReg UUserUnitGetInterpolatedPosition = {0x00E4DA64,
                                                   "GetInterpolatedPosition",
                                                   s_UserUnit,
                                                   "UserUnit:GetInterpolatedPosition()",
                                                   0x00000000,
                                                   GetInterpolatedPosition,
                                                   0x00F8D89C};

// PatcherList_UIFuncRegs_UUserUnitGetFractionComplete
luaFuncDescReg UUserUnitGetFractionComplete = {0x00E4DA64,
                                               "GetFractionComplete",
                                               s_UserUnit,
                                               "UserUnit:GetFractionComplete()",
                                               0x00000000,
                                               GetFractionComplete,
                                               0x00F8D89C};