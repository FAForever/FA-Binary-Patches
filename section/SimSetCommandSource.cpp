#include "moho.h"

int SimSetCommandSource(lua_State *L) {
    int armyI = lua_tonumber(L, 1);
    int sourceI = lua_tonumber(L, 2);
    bool set_or_unset = lua_toboolean(L, 3);

    auto srcSet = &g_Sim->armies[armyI]->mValidCommandSources;
    srcSet->set(sourceI, set_or_unset);
    return 0;
}

//PatcherList_SimFuncRegs_SSCSRegDesc
luaFuncDescReg SSCSRegDesc = {
    0x00E45E90,
    "SetCommandSource",
    0x00E00D90,
    "(targetArmyIndex, sourceHumanIndex, Set or Unset)",
    0x00000000,
    SimSetCommandSource,
    0x00000000};