#include <moho.h>

int SimSetFocusArmy(lua_State *L) {
    int armyIndex = lua_tonumber(L, 1);
    g_CSimDriver->focusArmyIndex = armyIndex;
    return 0;
}

//PatcherList_SimFuncRegs_SSFARegDesc
luaFuncDescReg SSFARegDesc = {
    0x00E45E90,
    0x00E43408, // "SetFocusArmy"
    0x00E00D90,
    0x00E451FC, // "SetFocusArmy(armyIndex or -1)"
    0x00000000,
    SimSetFocusArmy,
    0x00000000};