#include "include/CObject.h"
#include "include/moho.h"
#include "include/magic_classes.h"
#include <cmath>

#define NON_GENERAL_REG(var_) [var_] "g"(var_)

void _DrawRect(
    Vector3f *v1,
    Vector3f *v2,
    unsigned int color,
    float thickness,
    void *batcher,
    Vector3f *v3,
    void *heightmap,
    float f2)
{
    asm(
        "push %[f2];"
        "push %[heightmap];"
        "push %[v3];"
        "push %[batcher];"
        "movss xmm0, %[thickness];"
        "call 0x00455480;"
        "add esp, 0x10;"
        :
        : "c"(v2),
          "D"(color),
          "a"(v1),
          [thickness] "m"(thickness),
          NON_GENERAL_REG(f2),
          NON_GENERAL_REG(batcher),
          NON_GENERAL_REG(v3),
          NON_GENERAL_REG(heightmap)
        : "xmm0");
}

void DrawRect(
    Vector3f v1,
    Vector3f v2,
    uint32_t color,
    float f1,
    void *batcher,
    Vector3f v3,
    void *heightmap,
    float f2)
{
    return _DrawRect(&v1, &v2, color, f1, batcher, &v3, heightmap, f2);
}

float THICKNESS = 0.1;
extern unsigned int CIRCLE_COLOR;
void _DrawCircle(void *batcher, Vector3f *pos, float radius, float thickness, uint32_t color, Vector3f *orientation)
{
    THICKNESS = thickness;
    CIRCLE_COLOR = color;
    asm(
        "push %[radius];"
        "push %[pos];"
        "push %[batcher];"
        "call 0x00456200;"
        "add esp, 0xC;"
        :
        : "c"(orientation),
          NON_GENERAL_REG(batcher),
          NON_GENERAL_REG(pos),
          NON_GENERAL_REG(radius)
        : "eax");
}
namespace Moho
{
    void Import(lua_State *l, const char *filename)
    {
        lua_getglobal(l, "import");
        lua_pushstring(l, filename);
        lua_call(l, 1, 1);
    }

    void *GetWorldCamera()
    {
        int *camera = *(int **)((int)*((int *)g_WRenViewport + 2128) + 4);
        void *projmatrix = (*(void *(__thiscall **)(int *))(*camera + 8))(camera);
        return projmatrix;
    }
    namespace CPrimBatcher
    {
        void FlushBatcher(void *batcher)
        {
            asm(
                "push %[batcher];"
                "call 0x0043A140;"
                :
                : NON_GENERAL_REG(batcher)
                : "eax");
        }

        void ResetBatcher(void *batcher)
        {
            *(char *)((int *)batcher + 285) = 0;
        }

        struct Texture
        {
            int a;
            int b;
        };
        void __stdcall FromSolidColor(Texture *t, unsigned int color)
        {
            reinterpret_cast<void *(*)(Texture *, unsigned int)>(0x4478C0)(t, color);
        }

        Texture FromSolidColor(unsigned int color)
        {
            Texture t;
            FromSolidColor(&t, color);
            return t;
        }

        void __stdcall SetTexture(void *batcher, Texture *texture)
        {
            asm(
                "call 0x4386A0;"
                :
                : "D"(batcher),
                  "b"(texture)
                : "edx", "ecx", "eax");
        }

        void __stdcall SetViewProjMatrix(void *batcher, void *matrix)
        {
            asm(
                "push %[matrix];"
                "call 0x438640;"
                :
                : "b"(batcher),
                  NON_GENERAL_REG(matrix)
                : "edx", "eax");
        }
    } // namespace CPrimBatcher

    int *D3D_GetDevice()
    {
        return reinterpret_cast<int *(*)()>(0x00430590)();
    }

    void SetupDevice(int *device, const char *target, const char *mode)
    {
        (*(void(__thiscall **)(int *, const char *))(*device + 80))(device, target);
        (*(void(__thiscall **)(int *, const char *))(*device + 84))(device, mode);
    }

    bool TryConvertToColor(const char *s, uint32_t &color)
    {
        return reinterpret_cast<bool(__cdecl *)(const char *, uint32_t *)>(0x4B2B90)(s, &color);
    }

    float GetLODMetric(float *camera, const Vector3f &v)
    {
        return camera[169] * v.x + camera[170] * v.y + camera[171] * v.z + camera[172];
    }
} // namespace Moho

namespace IWldTerrainRes
{
    void *GetWldMap()
    {
        return (void *)g_CWldSession->map;
    }

    void *GetTerrainRes(void *wldmap)
    {
        return (void *)*((int *)wldmap + 1);
    }

    void *GetMap(void *wldTerrainRes)
    {
        return (void *)*((int *)wldTerrainRes + 1);
    }

} // namespace  IWldTerrainRes

// UI_Lua DrawRect()
int LuaDrawRect(lua_State *l)
{
    int *batcher = *(int **)(((int *)g_WRenViewport) + 2135);
    if (batcher == nullptr)
    {
        return 0;
    }
    float x, y, z;
    const char *s;
    x = luaL_checknumber(l, 1);
    y = luaL_checknumber(l, 2);
    z = luaL_checknumber(l, 3);
    s = lua_tostring(l, 4);
    uint32_t color;
    if (!Moho::TryConvertToColor(s, color))
    {
        lua_pushstring(l, "unknown color");
        lua_error(l);
        return 0;
    }
    Vector3f a{0, 0, 8};
    Vector3f b{8, 0, 0};
    Vector3f c{x, y, z};
    DrawRect(a, b, color, 1.f, batcher, c, nullptr, -10000);
    Moho::CPrimBatcher::FlushBatcher(batcher);
    return 0;
}

int LuaDrawCircle(lua_State *l)
{
    int *batcher = *(int **)(((int *)g_WRenViewport) + 2135);
    if (batcher == nullptr)
    {
        return 0;
    }
    float x, y, z, r;
    const char *s;
    x = luaL_checknumber(l, 1);
    y = luaL_checknumber(l, 2);
    z = luaL_checknumber(l, 3);
    r = luaL_checknumber(l, 4);
    s = lua_tostring(l, 5);
    float thickness = luaL_optnumber(l, 6, 0.15);
    uint32_t color;
    if (!Moho::TryConvertToColor(s, color))
    {
        lua_pushstring(l, "unknown color");
        lua_error(l);
        return 0;
    }

    Vector3f pos{x, y, z};
    Vector3f orientation{0, 1, 0};
    float lod = Moho::GetLODMetric((float *)Moho::GetWorldCamera(), pos);
    float a = std::max(thickness / lod, 2.f);
    _DrawCircle(batcher, &pos, r, lod * a, color, &orientation);

    Moho::CPrimBatcher::FlushBatcher(batcher);
    return 0;
}

bool CustomWorldRendering = false;
ConDescReg CustomWorldRenderingVar{"ui_CustomWorldRendering", "Enables custom world rendering", &CustomWorldRendering};


// this world view?
void __thiscall CustomDraw(void *_this, void *batcher)
{
    // void *wldmap = IWldTerrainRes::GetWldMap();
    // void *terrain = IWldTerrainRes::GetTerrainRes(wldmap);
    // if (!terrain)
    //     return;
    // void *map = IWldTerrainRes::GetMap(terrain);
    // if (!map)
    //     return;
    if (!CustomWorldRendering)
        return;

    LuaState *state = *(LuaState **)((int)g_CUIManager + 48);
    lua_State *l = state->m_state;
    Moho::Import(l, "/lua/ui/game/gamemain.lua");
    lua_pushstring(l, "OnRenderWorld");
    lua_rawget(l, -2);
    if (!lua_isfunction(l, -1))
    {
        WarningF("%s", "OnRenderWorld not a function");
        return;
    }
    int *device = Moho::D3D_GetDevice();
    Moho::SetupDevice(device, "primbatcher", "TAlphaBlendLinearSampleNoDepth");
    int *camera = *(int **)((int)_this + 4);
    void *projmatrix = (*(void *(__thiscall **)(int *))(*camera + 8))(camera);
    Moho::CPrimBatcher::ResetBatcher(batcher);
    Moho::CPrimBatcher::SetViewProjMatrix(batcher, projmatrix);
    Moho::CPrimBatcher::Texture t;
    Moho::CPrimBatcher::FromSolidColor(&t, 0xFFFFFFFF);
    Moho::CPrimBatcher::SetTexture(batcher, &t);

    float delta_frame = 1.0 / *(float *)(0xF57E74);
    lua_pushnumber(l, delta_frame);
    if (lua_pcall(l, 1, 0))
    {
        WarningF("%s", lua_tostring(l, -1));
    }
    Moho::CPrimBatcher::FlushBatcher(batcher);
}

void CustomDrawEnter()
{
    asm(
        "push edi;"
        "mov ecx, esi;"
        "call %[CustomDraw];"
        "pop     edi;" // as done in original code
        "pop     esi;"
        "pop     ebx;"
        "mov     esp, ebp;"
        "jmp     0x86EF30;" // jump back
        :
        : [CustomDraw] "i"(CustomDraw)
        :);
}
