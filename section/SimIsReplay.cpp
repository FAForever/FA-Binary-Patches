#include "moho.h"
#include "magic_classes.h"

int SimSessionIsReplay(lua_State *L) {
  lua_pushboolean(L, g_CWldSession->isReplay);
  return 1;
}

SimRegFunc SSIRRegDesc{(const char *)0x00E4AFBC, (const char *)0x00E4AF84,
                       SimSessionIsReplay};