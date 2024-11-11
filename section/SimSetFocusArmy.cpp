#include "magic_classes.h"
#include "moho.h"


int SimSetFocusArmy(lua_State *L) {
  int armyIndex = lua_tonumber(L, 1);
  g_CSimDriver->focusArmyIndex = armyIndex;
  return 0;
}

SimRegFunc SimSetFocusArmyReg{(const char *)0x00E43408,
                              (const char *)0x00E451FC, SimSetFocusArmy};