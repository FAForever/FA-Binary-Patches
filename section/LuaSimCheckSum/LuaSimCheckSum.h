#include "global.h"
#include "LuaAPI.h"

typedef int (*lua_Chunkwriter)(lua_State *L, const void *p,
                               size_t sz, void *ud);

int __cdecl lua_pcallF(lua_State *L, int nargs, int nResults) asm("0x0090D430");
int __cdecl lua_dump(lua_State *L, lua_Chunkwriter writer, void *ud) asm("0x0090D610");
string *__thiscall gpg__MD5Digest__ToString(void *_this, string *result) asm("0x008E5910");
void __thiscall gpg__MD5Context__Update(/* gpg::MD5Context */ void *_this, const void *block, size_t size) asm("0x008E5870");