#include "CObject.h"
#include "magic_classes.h"
#include "moho.h"
#include "global.h"
#define NON_GENERAL_REG(var_) [var_] "g"(var_)

float navPersonalPosMaxDistance = 50.0;


int SetNavIndividualPosMaxDistance(lua_State *L) {
    navPersonalPosMaxDistance = luaL_optnumber(L, 1, 50.0);

    return 0;
}
SimRegFunc SetNavDistanceReg{"SetNavigatorPersonalPosMaxDistance", "", SetNavIndividualPosMaxDistance};