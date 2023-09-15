#include "include/moho.h"

#define lua_push(L, name, val) \
    lua_pushstring(L, name); \
    lua_pushnumber(L, val); \
    lua_rawset(L, -3);

int SimGetDepositsAroundPoint(lua_State *L) {
    float X = lua_tonumber(L, 1);
    float Z = lua_tonumber(L, 2);
    float Radius = lua_tonumber(L, 3);

    if (X + Z + Radius != X + Z + Radius) return 0;

    int Type = lua_tonumber(L, 4);
    Radius *= Radius;

    lua_newtable(L);
    int i = 1;

    auto deposit = g_Sim->res->deposits.begin;
    auto endDeposit = g_Sim->res->deposits.end;
    for (; deposit < endDeposit; deposit++) {
        if (Type && Type != deposit->Type) continue;
        float x = (deposit->X2 + deposit->X1) * 0.5f - X;
        float z = (deposit->Z2 + deposit->Z1) * 0.5f - Z;
        float dist = x * x + z * z;
        if (dist > Radius) continue;
        lua_pushnumber(L, i++);
        lua_newtable(L);
        lua_push(L, "X1", deposit->X1);
        lua_push(L, "Z1", deposit->Z1);
        lua_push(L, "X2", deposit->X2);
        lua_push(L, "Z2", deposit->Z2);
        lua_push(L, "Type", deposit->Type);
        lua_push(L, "Dist", sqrtf(dist));
        lua_settable(L, -3);
    }
    return 1;
}

#define s_GDAPName "GetDepositsAroundPoint"
#define s_GDAPDesc "(X, Z, Radius, Type)"
//PatcherList_SimFuncRegs_SGDAPRegDesc
luaFuncDescReg SGDAPRegDesc = {
    0x00E45E90,
    s_GDAPName,
    0x00E00D90,
    s_GDAPDesc,
    0x00000000,
    SimGetDepositsAroundPoint,
    0x00000000};

//PatcherList_UIFuncRegs_UGDAPRegDesc
luaFuncDescReg UGDAPRegDesc = {
    0x00E45E90,
    s_GDAPName,
    0x00E00D90,
    s_GDAPDesc,
    0x00000000,
    SimGetDepositsAroundPoint,
    0x00000000};