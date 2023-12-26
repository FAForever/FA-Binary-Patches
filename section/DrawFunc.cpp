#include "include/CObject.h"
#include "include/magic_classes.h"
#include "include/moho.h"
#include "include/utility.h"
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

    void *GetWorldCamera(void *worldview)
    {
        int *camera = *(int **)((int)worldview + 4);
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

        void __stdcall FromSolidColor(Texture *t, unsigned int color) asm("0x4478C0");

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

    int *D3D_GetDevice() asm("0x00430590");

    void SetupDevice(int *device, const char *target, const char *mode)
    {
        (*(void(__thiscall **)(int *, const char *))(*device + 80))(device, target);
        (*(void(__thiscall **)(int *, const char *))(*device + 84))(device, mode);
    }

    bool __cdecl TryConvertToColor(const char *s, uint32_t &color) asm("0x4B2B90");

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
bool is_in_render_world = false;

void *worldview = nullptr;
// UI_Lua DrawRect()
int LuaDrawRect(lua_State *l)
{
    int *batcher = *(int **)(((int *)g_WRenViewport) + 2135);
    if (batcher == nullptr || worldview == nullptr)
    {
        return 0;
    }
    if (!is_in_render_world)
    {
        luaL_error(l, "Attempt to call DrawRect outside of OnRenderWorld");
        return 0;
    }

    Vector3f pos = ToVector(l, 1);
    float size = luaL_checknumber(l, 2);
    const char *s = lua_tostring(l, 3);
    float thickness = luaL_optnumber(l, 4, 0.15);
    uint32_t color;
    if (!Moho::TryConvertToColor(s, color))
    {
        luaL_error(l, s_UnknownColor, s);
        return 0;
    }
    Vector3f orientation{0, 1, 0};
    float lod = Moho::GetLODMetric((float *)Moho::GetWorldCamera(worldview), pos);
    float thick = std::max(thickness / lod, 2.f);
    Vector3f a{0, 0, size};
    Vector3f b{size, 0, 0};
    DrawRect(a, b, color, thick * lod, batcher, pos, nullptr, -10000);
    Moho::CPrimBatcher::FlushBatcher(batcher);
    return 0;
}
UIRegFunc DrawRectReg{"UI_DrawRect", "UI_DrawRect(pos:vector, size:float, color:string, thickness?=0.15:float)", LuaDrawRect};

int LuaDrawCircle(lua_State *l)
{
    int *batcher = *(int **)(((int *)g_WRenViewport) + 2135);
    if (batcher == nullptr || worldview == nullptr)
    {
        return 0;
    }
    if (!is_in_render_world)
    {
        luaL_error(l, "Attempt to call DrawCircle outside of OnRenderWorld");
        return 0;
    }
    Vector3f pos = ToVector(l, 1);
    float r = luaL_checknumber(l, 2);
    const char *s = lua_tostring(l, 3);
    float thickness = luaL_optnumber(l, 4, 0.15);
    uint32_t color;
    if (!Moho::TryConvertToColor(s, color))
    {
        luaL_error(l, s_UnknownColor, s);
        return 0;
    }
    Vector3f orientation{0, 1, 0};
    float lod = Moho::GetLODMetric((float *)Moho::GetWorldCamera(worldview), pos);
    float a = std::max(thickness / lod, 2.f);
    _DrawCircle(batcher, &pos, r, lod * a, color, &orientation);

    Moho::CPrimBatcher::FlushBatcher(batcher);
    return 0;
}

UIRegFunc DrawCircleReg{"UI_DrawCircle", "UI_DrawCircle(pos:vector, radius:float, color:string, thickness?=0.15:float)", LuaDrawCircle};

bool CustomWorldRendering = false;
ConDescReg CustomWorldRenderingVar{"ui_CustomWorldRendering", "Enables custom world rendering", &CustomWorldRendering};

float delta_frame = 0;

// offset +284 from CUIWorldView
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
    worldview = _this;

    // LogF("%p", *(int *)(0x010A6470));//gamesession
    // LogF("%p", *(int *)((int)worldview - 284 + 532));

    LuaState *state = *(LuaState **)((int)g_CUIManager + 48);
    lua_State *l = state->m_state;
    LuaObject *view = (LuaObject *)((int)worldview - 284 + 32);
    if (!*(char *)((int)worldview + 377 - 284))//check for custom render enabled (see WorldView.cpp)
        return;
    // Moho::Import(l, "/lua/ui/game/gamemain.lua");
    view->PushStack(l);
    lua_pushstring(l, "OnRenderWorld");
    lua_gettable(l, -2);
    if (!lua_isfunction(l, -1))
    {
        // WarningF("%s", "OnRenderWorld not a function");
        return;
    }
    int *device = Moho::D3D_GetDevice();
    Moho::SetupDevice(device, "primbatcher", "TAlphaBlendLinearSampleNoDepth");
    Moho::CPrimBatcher::ResetBatcher(batcher);
    Moho::CPrimBatcher::SetViewProjMatrix(batcher, Moho::GetWorldCamera(worldview));
    Moho::CPrimBatcher::Texture t;
    Moho::CPrimBatcher::FromSolidColor(&t, 0xFFFFFFFF);
    Moho::CPrimBatcher::SetTexture(batcher, &t);

    is_in_render_world = true;
    lua_pushvalue(l, -2);
    lua_pushnumber(l, delta_frame);
    if (lua_pcall(l, 2, 0))
    {
        WarningF("%s", lua_tostring(l, -1));
    }
    lua_pop(l, 1);
    is_in_render_world = false;
    Moho::CPrimBatcher::FlushBatcher(batcher);
    worldview = nullptr;
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

void StoreFPS()
{
    asm(
        "movss  xmm0, dword ptr [esp+0x78-0x68];"
        "movss ds:%[delta_frame], xmm0;"
        "jmp 0x8D1A8B;"
        :
        : [delta_frame] "i"(&delta_frame)
        :);
}