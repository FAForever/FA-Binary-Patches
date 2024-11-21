#include <LuaAPI.h>

VALIDATE_SIZE(lua::Table, 0x24);

extern const luaL_reg original_table_funcs[] asm("0x00D47418");

SHARED int __cdecl lua_openlibtable(lua_State *L);