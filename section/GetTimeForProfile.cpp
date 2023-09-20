#include <moho.h>

int GetTimeForProfile(lua_State *L) {
    int64_t Time, Freq;
    QueryPerformanceCounter(&Time);
    QueryPerformanceFrequency(&Freq);
    float OriginTime = lua_tonumber(L, 1);
    float r = (Time - OriginTime * Freq) / Freq;
    lua_pushnumber(L, r);
    return 1;
}

#define s_GTFPName "GetTimeForProfile"
#define s_GTFPDesc "(OriginTime)"
//PatcherList_SimFuncRegs_SGTFPRegDesc
luaFuncDescReg SGTFPRegDesc = {
    0x00E45E90,
    s_GTFPName,
    0x00E00D90,
    s_GTFPDesc,
    0x00000000,
    GetTimeForProfile,
    0x00000000};

//PatcherList_UIFuncRegs_UGTFPRegDesc
luaFuncDescReg UGTFPRegDesc = {
    0x00E45E90,
    s_GTFPName,
    0x00E00D90,
    s_GTFPDesc,
    0x00000000,
    GetTimeForProfile,
    0x00000000};