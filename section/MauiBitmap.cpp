#include "include/CObject.h"
#include "include/LuaApi.h"
#include "include/global.h"
#include "include/magic_classes.h"

bool __cdecl TryConvertToColor(const char *s, uint32_t &color) asm("0x4B2B90");

int SetColorMask(lua_State *l)
{
    if (lua_gettop(l) != 2)
    {
        l->LuaState->Error(s_ExpectedButGot, __FUNCTION__, 2, lua_gettop(l));
    }

    auto r = GetCScriptObject<CMauiBitmap>(l, 1);
    if (r.IsFail())
    {
        lua_pushstring(l, r.reason);
        lua_error(l);
        return 0;
    }
    void *bitmap = r.object;
    if (bitmap == nullptr)
        return 0;
    const char *s = lua_tostring(l, 2);
    uint32_t color;
    if (!TryConvertToColor(s, color))
    {
        luaL_error(l, s_UnknownColor, s);
        return 0;
    }
    color &= 0x00FFFFFFu;
    *(uint32_t *)((int)bitmap + 244) = (*(uint32_t *)((int)bitmap + 244) & 0xFF000000u) | color;
    return 0;
}

// PatcherList_UIFuncRegs_BitmapSetColorMaskDesc
luaFuncDescReg BitmapSetColorMaskDesc = {0x00E37C14,
                                         "SetColorMask",
                                         "CMauiBitmap",
                                         "Bitmap:SetColorMask(color)",
                                         nullptr,
                                         SetColorMask,
                                         0x00F8D7DC};

