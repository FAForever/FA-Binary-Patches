#pragma once
#include <cstdint>

#define VALIDATE_SIZE(struc, size) \
    static_assert(sizeof(struc) == size, "Invalid structure size of " #struc);

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TTABLE 5
#define LUA_CFUNCTION 6
#define LUA_TFUNCTION 7
#define LUA_TUSERDATA 8
#define LUA_TTHREAD 9
#define LUA_TPROTO 10
#define LUA_TUPVALUE 11

#define ttype(o) ((o)->tt)
#define ttisnil(o) (ttype(o) == LUA_TNIL)
#define ttisnumber(o) (ttype(o) == LUA_TNUMBER)
#define ttisstring(o) (ttype(o) == LUA_TSTRING)
#define ttiswstring(o) (ttype(o) == LUA_TWSTRING)
#define ttistable(o) (ttype(o) == LUA_TTABLE)
#define ttisfunction(o) (ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o) (ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o) (ttype(o) == LUA_TUSERDATA)
#define ttisthread(o) (ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o) (ttype(o) == LUA_TLIGHTUSERDATA)

#define CommonHeader \
    GCObject *next;  \
    uint8_t tt;      \
    uint8_t marked

#define MAXTAGLOOP 100

namespace lua
{
    typedef struct lua_State lua_State;

    typedef enum
    {
        TM_INDEX,
        TM_NEWINDEX,
        TM_GC,
        TM_MODE,
        TM_EQ, /* last tag method with `fast' access */
        TM_ADD,
        TM_SUB,
        TM_MUL,
        TM_DIV,
        TM_POW,
        TM_UNM,
        TM_LT,
        TM_LE,
        TM_CONCAT,
        TM_CALL,
        TM_N /* number of elements in the enum */
    } TMS;

    struct TObject
    {
        int tt;
        void *value;
    };

    struct Node
    {
        TObject i_key;
        TObject i_val;
        Node *next;
    };

    typedef struct GCObject GCObject;

    struct Table
    {
        CommonHeader;
        uint16_t gap;
        uint8_t flags;
        uint8_t lsizenode;
        // padding byte
        // padding byte
        Table *metatable;
        TObject *array;
        lua::Node *node;
        lua::Node *firstfree;
        /*lua::GCObject*/ void *gclist;
        int sizearray;
    };

    typedef TObject *StkId;

    typedef struct global_State
    {
        char strt[0xC];
        lua::GCObject *rootgc;
        lua::GCObject *rootgc1;
        lua::GCObject *rootudata;
        lua::GCObject *tmudata;
        char buff[8];
        int GCthreshold;
        void *panic;
        int nblocks;
        lua::TObject _registry;
        lua::TObject _defaultmeta;
        lua::lua_State *mainthread;
        lua::lua_State *lstate;
        lua::Node dummynode[1];
        void *tmname[15];
        char pad[288];
    } global_State;

    typedef struct lua_State
    {
        CommonHeader;
        lua::TObject *top;
        lua::StkId base;
        global_State *l_G;
        void *ci;
        lua::StkId stack_last;
        char pad[0x40 + 20];
    } lua_State;

} // namespace lua
// const int a = sizeof(lua::global_State);
VALIDATE_SIZE(lua::lua_State, 0x70);
VALIDATE_SIZE(lua::global_State, 0x1B8);
static_assert(offsetof(lua::global_State, tmname) == 0x5c, "Invalid tmname offset");

#define setobj(obj1, obj2)               \
    {                                    \
        const lua::TObject *o2 = (obj2); \
        lua::TObject *o1 = (obj1);       \
        o1->tt = o2->tt;                 \
        o1->value = o2->value;           \
    }

#define luaD_checkstack(L, n)                                                      \
    if ((char *)L->stack_last - (char *)L->top <= (n) * (int)sizeof(lua::TObject)) \
        luaD_growstack(L, n);

#define G(L) (L->l_G)
#define gfasttm(g, et, e) \
    (((et)->flags & (1u << (e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l, et, e) gfasttm(G(l), et, e)

const lua::TObject *__cdecl luaT_gettm(lua::Table *events, char event, void *ename) asm("0x00928420");
lua::lua_State *__cdecl luaD_growstack(lua::lua_State *L, int n) asm("0x00913990");
void __cdecl luaD_call(lua::lua_State *L, lua::StkId func, int nResults) asm("0x00914430");
lua::TObject *__cdecl luaH_set(lua::lua_State *L, lua::Table *t, const lua::TObject *key) asm("0x00927560");
const lua::TObject *__cdecl luaT_gettmbyobj(lua::lua_State *L, const lua::TObject *o, lua::TMS event) asm("0x00928450");
void luaG_runerror(lua::lua_State *L, const char *msg, ...) asm("0x00913270");
void __cdecl luaG_typeerror(lua::lua_State *L, const lua::TObject *obj, const char *oper) asm("0x009133A0");