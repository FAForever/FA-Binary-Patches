#include "moho.h"
#include "magic_classes.h"

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


UIRegFunc SGTFPRegDesc{s_GTFPName, "(OriginTime)", GetTimeForProfile};
SimRegFunc UGTFPRegDesc{s_GTFPName, "(OriginTime)", GetTimeForProfile};