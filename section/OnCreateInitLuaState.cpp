#include "include/moho.h"

void __thiscall OnCreateInitLuaState(LuaState* state, int enumStdLibs) {
    reinterpret_cast<void(__thiscall*)(LuaState*, int)>(0x90AC10)(state, enumStdLibs); //original

    lua_register(state->m_state, "GetProcessAffinityMask", [](lua_State *L) -> int {
        //bool, uint32_t, uint32_t = GetProcessAffinityMask()
        auto GetProcessAffinityMask = *reinterpret_cast<int(__stdcall**)(void*, uint32_t*, uint32_t*)>(0x00C0F584);
        uint32_t dwProcessAffinityMask;
        uint32_t dwSystemAffinityMask;
        int res = GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinityMask, &dwSystemAffinityMask);
        lua_pushboolean(L, res != 0);
        lua_pushnumber(L, dwProcessAffinityMask);
        lua_pushnumber(L, dwSystemAffinityMask);
        return 3;
    });

    lua_register(state->m_state, "SetProcessAffinityMask", [](lua_State *L) -> int {
        //bool SetProcessAffinityMask(uint32_t dwProcessAffinityMask)
        uint32_t dwProcessAffinityMask = luaL_checknumber(L, 1);
        auto SetProcessAffinityMask = *reinterpret_cast<int(__stdcall**)(void*, uint32_t)>(0x00C0F444);
        int res = SetProcessAffinityMask(GetCurrentProcess(), dwProcessAffinityMask);
        lua_pushboolean(L, res != 0);
        return 1;
    });

    lua_register(state->m_state, "SetProcessPriority", [](lua_State *L) -> int {
        //bool SetProcessPriority(uint32_t dwPriorityClass)
        uint32_t dwPriorityClass = luaL_checknumber(L, 1);
        auto SetPriorityClass = reinterpret_cast<uint32_t(__stdcall*)(void*, uint32_t)>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetPriorityClass"));
        int res = SetPriorityClass(GetCurrentProcess(), dwPriorityClass);
        lua_pushboolean(L, res != 0);
        return 1;
    });
}
