#include "include/CObject.h"
#include "include/magic_classes.h"
#include "include/moho.h"
#include "include/utility.h"
#include <cmath>

bool __cdecl WIN_CopyToClipboard(const wchar_t *Src) asm("0x004F2730");

int UI_CopyToClipboard(lua_State *l)
{
    if (lua_gettop(l) != 1)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 1, lua_gettop(l));
    }
    const char *s = lua_tostring(l, 1);
    wstring ws(s);
    lua_pushboolean(l, WIN_CopyToClipboard(ws.data()));
    return 1;
}

UIRegFunc copyToClipboardReg{"UI_CopyToClipboard", "", UI_CopyToClipboard};