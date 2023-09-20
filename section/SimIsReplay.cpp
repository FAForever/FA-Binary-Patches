#include <moho.h>

int SimSessionIsReplay(lua_State *L) {
    lua_pushboolean(L, g_CWldSession->isReplay);
    return 1;
}

//PatcherList_SimFuncRegs_SSIRRegDesc
luaFuncDescReg SSIRRegDesc = {
    0x00E45E90,
    0x00E4AFBC, // "SessionIsReplay"
    0x00E00D90,
    0x00E4AF84, // "Return true if the active session is a replay session."
    0x00000000,
    SimSessionIsReplay,
    0x00000000};