#include "include/CObject.h"
#include "include/magic_classes.h"
#include "include/moho.h"
#include "include/utility.h"

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
    PushVector(l, {mesh[34], mesh[35], mesh[36]});
    return 1;
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

using UserUnitMethodReg = UIRegFunc<0x00E4DA64, 0x00F8D89C>;

UserUnitMethodReg UserUnitGetInterpolatedPosition{
    "GetInterpolatedPosition",
    "UserUnit:GetInterpolatedPosition()",
    GetInterpolatedPosition,
    s_UserUnit};

UserUnitMethodReg UserUnitGetFractionComplete{
    "GetFractionComplete",
    "UserUnit:GetFractionComplete()",
    GetFractionComplete,
    s_UserUnit};
