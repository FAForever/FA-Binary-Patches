#include "LuaAPI.h"
#include <new>

void *__cdecl luaHelper_ReallocFunction(void *ptr, unsigned int oldsize, unsigned int size, void *data, const char *allocName, unsigned int flags) asm("0x923F20");
void __cdecl luaHelper_FreeFunction(void *ptr, unsigned int oldsize, void *data) asm("0x923F40");

typedef void *(__cdecl *lua_ReallocFunction)(void *ptr, unsigned int oldsize, unsigned int size, void *data, const char *allocName, unsigned int flags);
typedef void(__cdecl *lua_FreeFunction)(void *ptr, unsigned int oldsize, void *data);

void __cdecl lua_setdefaultmemoryfunctions(lua_ReallocFunction reallocFunc, lua_FreeFunction freeFunc, void *data) asm("0x923F80");
void __cdecl lua_getdefaultmemoryfunctions(lua_ReallocFunction *reallocFunc, lua_FreeFunction *freeFunc, void **data) asm("0x923F50");

int64_t __cdecl ClockCycles() asm("0x00955400");
uint64_t __cdecl CyclesToMicroSeconds(int64_t cycles) asm("0x00955520");