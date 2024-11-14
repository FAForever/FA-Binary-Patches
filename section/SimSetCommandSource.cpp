#include "moho.h"
#include "magic_classes.h"

int SimSetCommandSource(lua_State *L) {
  int armyI = lua_tonumber(L, 1);
  int sourceI = lua_tonumber(L, 2);
  bool set_or_unset = lua_toboolean(L, 3);

  auto srcSet = &g_Sim->armies[armyI]->mValidCommandSources;
  srcSet->set(sourceI, set_or_unset);
  return 0;
}

SimRegFunc SSCSRegDesc{"SetCommandSource",
                       "(targetArmyIndex, sourceHumanIndex, Set or Unset)",
                       SimSetCommandSource};