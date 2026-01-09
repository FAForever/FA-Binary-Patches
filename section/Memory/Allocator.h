#include "LuaAPI.h"

void *__cdecl luaHelper_ReallocFunction(void *ptr, unsigned int oldsize, unsigned int size, void *data, const char *allocName, unsigned int flags) asm("0x923F20");
void __cdecl luaHelper_FreeFunction(void *ptr, unsigned int oldsize, void *data) asm("0x923F40");

typedef void *(__cdecl *lua_ReallocFunction)(void *ptr, unsigned int oldsize, unsigned int size, void *data, const char *allocName, unsigned int flags);
typedef void(__cdecl *lua_FreeFunction)(void *ptr, unsigned int oldsize, void *data);

void __cdecl lua_setdefaultmemoryfunctions(lua_ReallocFunction reallocFunc, lua_FreeFunction freeFunc, void *data) asm("0x923F80");
void __cdecl lua_getdefaultmemoryfunctions(lua_ReallocFunction *reallocFunc, lua_FreeFunction *freeFunc, void **data) asm("0x923F50");