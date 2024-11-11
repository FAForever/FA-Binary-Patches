#include "LuaAPI.h"

void GetTableAH(const void *t, uint32_t *asize, uint8_t *hbits) {
    *asize = *(int*)(t + 32);
    *hbits = *(uint8_t*)(t + 9);
}

// struct LuaExts {
//     LuaExts() {
//         if (FAJit) {
//             WarningF("%s", "Used FAExt.dll");
//             lua_createtable = GetProcAddress(FAJit, "lua_createtable");
//             GetTableAH = GetProcAddress(FAJit, "GetTableAH");
//         } else {
//             extern void *LCreateTable[];
//             if (LCreateTable[0])
//                 lua_createtable = LCreateTable[0];
//             GetTableAH = e_GetTableAH;
//         }
//     }
// } luaexts;