#include "LuaSimCheckSum.h"

int Md5_Writer(lua_State *L, const void *p, size_t sz, void *ud)
{
    gpg__MD5Context__Update(ud, p, sz);
    return 0;
}

SHARED void __cdecl lua_dochecksum(lua_State *L, int, int)
{
    void *sim = lua_getglobaluserdata(L);
    if (sim == nullptr)
    {
        lua_call(L, 0, 0);
        return;
    }
    void *md5 = Offset(sim, 0x50);
    lua_dump(L, Md5_Writer, md5);
    // string s;
    // gpg__MD5Digest__ToString(md5, &s);
    // LogF("Checksum: %s", s.data());
    lua_call(L, 0, 0);
    return;
}
