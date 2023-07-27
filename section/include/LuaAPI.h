#pragma once

#include "global.h"

#define LUA_IDSIZE	60

struct lua_Debug {
  int event;
  const char *name;		// (n)
  const char *namewhat;		// (n) 'global', 'local', 'field', 'method'
  const char *what;		// (S) 'Lua', 'C', 'main', 'tail'
  const char *source;		// (S)
  int currentline;		// (l)
  int nups;			// (u) number of upvalues
  int linedefined;		// (S)
  char short_src[LUA_IDSIZE];	// (S)
  /* private part */
  int i_ci;  // active function
};

class LuaState;
//lua.org/source/5.0/lstate.h.html#lua_State
typedef struct {uint8_t pad[0x44]; LuaState* LuaState;} lua_State;

typedef struct luaL_Buffer luaL_Buffer;
typedef const char* (*lua_Chunkreader)(lua_State *L, void *data, size_t *size);
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);
typedef void (*userGCFunction)(void*);
typedef int (*lua_CFunction)(lua_State *L);
typedef float lua_Number;

typedef struct luaL_reg {
  const char *name;
  lua_CFunction func;
} luaL_reg;

#define LUA_MULTRET	(-1)

#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10001)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))

#define LUA_OK		0
#define LUA_ERRRUN	1
#define LUA_ERRFILE	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5

#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET	4

#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

#define LUA_NOREF	(-2)
#define LUA_REFNIL	(-1)

#define lua_boxpointer(L,u) \
  (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)   (*(void **)(lua_touserdata(L, i)))

#define lua_pop(L,n)            lua_settop(L, -(n)-1)

#define lua_register(L,n,f) \
  (lua_pushstring(L, n), \
  lua_pushcfunction(L, f), \
  lua_settable(L, LUA_GLOBALSINDEX))

#define lua_pushcfunction(L,f)  lua_pushcclosure(L, f, 0)

#define lua_isfunction(L,n)     (lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)        (lua_type(L,n) == LUA_TTABLE)
#define lua_islightuserdata(L,n)(lua_type(L,n) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)          (lua_type(L,n) == LUA_TNIL)
#define lua_isboolean(L,n)      (lua_type(L,n) == LUA_TBOOLEAN)
#define lua_isnone(L,n)         (lua_type(L,n) == LUA_TNONE)
#define lua_isnoneornil(L, n)   (lua_type(L,n) <= 0)

#define lua_pushliteral(L, s) \
  lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_getregistry(L)      lua_pushvalue(L, LUA_REGISTRYINDEX)
#define lua_setglobal(L,s) \
  (lua_pushstring(L, s), lua_insert(L, -2), lua_settable(L, LUA_GLOBALSINDEX))
#define lua_getglobal(L,s) \
  (lua_pushstring(L, s), lua_gettable(L, LUA_GLOBALSINDEX))

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
  (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))
#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))
#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, ref)

typedef union {
  void *p;
  float n;
  int b;
} Value;

//lua.org/source/5.0/lobject.h.html#TObject
typedef struct {
  int tt;
  Value value;

  /* Types:
   -1 - None
    0 - Nil
    1 - Boolean
    2 - LightUserData
    3 - Number
    4 - String
    5 - Table
    6 - CFunction
    7 - Function
    8 - UserData
    9 - Thread
  */
} TObject;
VALIDATE_SIZE(TObject, 8)

//namespace gpg
  class RRef {public: void* d; void* t;};
  class RType {};

//namespace LuaPlus
  class LuaState;

  class LuaStackObject {
    public:
      LuaState* m_state;
      int m_stackIndex;
  };
  VALIDATE_SIZE(LuaStackObject, 8)

  class LuaObject
  {// 0x14 bytes
    public:
      LuaObject* m_next;
      LuaObject* m_prev;
      LuaState* m_state;
      TObject m_object;
  };
  VALIDATE_SIZE(LuaObject, 0x14)

  namespace CLuaObject
  {
    //public
      FDecl(0x9072a0, CLuaObject, __thiscall LuaObject (*)(LuaObject* this_))
      FDecl(0x908970, CLuaObject2, __thiscall LuaObject (*)(LuaObject* this_, LuaState*))
      FDecl(0x9089c0, CLuaObject3, __thiscall LuaObject (*)(LuaObject* this_, LuaState*, int))
      FDecl(0x908a40, CLuaObject4, __thiscall LuaObject (*)(LuaObject* this_, const LuaObject&))
      FDecl(0x908a70, CLuaObject5, __thiscall LuaObject (*)(LuaObject* this_, const LuaStackObject&))
      FDecl(0x9089f0, CLuaObject6, __thiscall LuaObject (*)(LuaObject* this_, LuaState*, const TObject*))
      FDecl(0x9075d0, DLuaObject, __thiscall void (*)(LuaObject* this_))
      FDecl(0x908ab0, EQ, __thiscall LuaObject& (*)(LuaObject* this_, const LuaObject&))
      FDecl(0x908b00, EQ2, __thiscall LuaObject& (*)(LuaObject* this_, const LuaStackObject&))
      FDecl(0x9091e0, Get, __thiscall LuaObject (*)(LuaObject* this_, int))
      FDecl(0x908f60, Get2, __thiscall LuaObject (*)(LuaObject* this_, const char*))
      FDecl(0x907c90, GetBoolean, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x9078d0, IsBoolean, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x9077c0, IsConvertibleToString, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907810, IsFunction, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907350, IsInteger, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x9072f0, IsNil, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907360, IsNumber, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907370, IsString, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907310, IsTable, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x907320, IsUserData, __thiscall bool (*)(LuaObject* this_))
      FDecl(0x90a180, Clone, __thiscall LuaObject (*)(LuaObject* this_))
      FDecl(0x908c10, CreateTable, __thiscall LuaObject (*)(LuaObject* this_, const char*, int, int))
      FDecl(0x908ca0, CreateTable2, __thiscall LuaObject (*)(LuaObject* this_, int, int, int))
      FDecl(0x908df0, GetByIndex, __thiscall LuaObject (*)(LuaObject* this_, int))
      FDecl(0x90a160, GetByName, __thiscall LuaObject (*)(LuaObject* this_, const char*))
      FDecl(0x908e70, GetByObject, __thiscall LuaObject (*)(LuaObject* this_, const LuaObject&))
      FDecl(0x908ba0, GetMetaTable, __thiscall LuaObject (*)(LuaObject* this_))
      FDecl(0x9093b0, Lookup, __thiscall LuaObject (*)(LuaObject* this_, const char*))
      FDecl(0x907d80, PushStack, __thiscall void (*)(LuaObject* this_, LuaStackObject* out, LuaState*))
      FDecl(0x907d10, PushStack2, __thiscall void (*)(LuaObject* this_, lua_State*))
      FDecl(0x9072b0, GetActiveState, __thiscall LuaState* (*)(LuaObject* this_))
      FDecl(0x907a90, GetString, __thiscall const char* (*)(LuaObject* this_))
      FDecl(0x9073e0, ToString, __thiscall const char* (*)(LuaObject* this_))
      FDecl(0x908b50, TypeName, __thiscall const char* (*)(LuaObject* this_))
      FDecl(0x907970, GetNumber, __thiscall float (*)(LuaObject* this_))
      FDecl(0x907a30, GetNumber2, __thiscall float (*)(LuaObject* this_))
      FDecl(0x9073b0, ToNumber, __thiscall float (*)(LuaObject* this_))
      FDecl(0x909840, AssignNewUserData, __thiscall RRef (*)(LuaObject* this_, LuaState*, const RRef&))
      FDecl(0x9097d0, AssignNewUserData2, __thiscall RRef (*)(LuaObject* this_, LuaState*, const RType*))
      FDecl(0x907bc0, GetUserData, __thiscall RRef (*)(LuaObject* this_))
      FDecl(0x907f50, GetCount, __thiscall int (*)(LuaObject* this_))
      FDecl(0x907910, GetInteger, __thiscall int (*)(LuaObject* this_))
      FDecl(0x907e50, GetN, __thiscall int (*)(LuaObject* this_))
      FDecl(0x90a410, GetTableCount, __thiscall int (*)(LuaObject* this_))
      FDecl(0x907440, IsPassed, __thiscall int (*)(LuaObject* this_))
      FDecl(0x9076d0, Type, __thiscall int (*)(LuaObject* this_))
      FDecl(0x9072c0, GetActiveCState, __thiscall lua_State* (*)(LuaObject* this_))
      FDecl(0x908890, AddToUsedList, __thiscall void (*)(LuaObject* this_, LuaState*))
      FDecl(0x9088e0, AddToUsedList2, __thiscall void (*)(LuaObject* this_, LuaState*, const TObject&))
      FDecl(0x909600, AssignBoolean, __thiscall void (*)(LuaObject* this_, LuaState*, bool))
      FDecl(0x909650, AssignInteger, __thiscall void (*)(LuaObject* this_, LuaState*, int))
      FDecl(0x909940, AssignNewTable, __thiscall void (*)(LuaObject* this_, LuaState*, int, int))
      FDecl(0x9095c0, AssignNil, __thiscall void (*)(LuaObject* this_, LuaState*))
      FDecl(0x9096a0, AssignNumber, __thiscall void (*)(LuaObject* this_, LuaState*, float))
      FDecl(0x909750, AssignString, __thiscall void (*)(LuaObject* this_, LuaState*, const char*))
      FDecl(0x9099b0, AssignTObject, __thiscall void (*)(LuaObject* this_, LuaState*, const TObject*))
      FDecl(0x9096f0, AssignThread, __thiscall void (*)(LuaObject* this_, LuaState*))
      FDecl(0x909af0, Insert, __thiscall void (*)(LuaObject* this_, LuaObject*))
      FDecl(0x909ce0, Insert2, __thiscall void (*)(LuaObject* this_, int, LuaObject*))
      FDecl(0x907630, Register, __thiscall void (*)(LuaObject* this_, const char*, int (__cdecl*)(lua_State*), int))
      FDecl(0x9075f0, Reset, __thiscall void (*)(LuaObject* this_))
      FDecl(0x9080c0, SetBoolean, __thiscall void (*)(LuaObject* this_, const char*, bool))
      FDecl(0x9081f0, SetInteger, __thiscall void (*)(LuaObject* this_, const char*, int))
      FDecl(0x908240, SetInteger2, __thiscall void (*)(LuaObject* this_, int, int))
      FDecl(0x907e00, SetMetaTable, __thiscall void (*)(LuaObject* this_, const LuaObject&))
      FDecl(0x907ed0, SetN, __thiscall void (*)(LuaObject* this_, int))
      FDecl(0x907fa0, SetNil, __thiscall void (*)(LuaObject* this_, const char*))
      FDecl(0x907ff0, SetNil2, __thiscall void (*)(LuaObject* this_, int))
      FDecl(0x908320, SetNumber, __thiscall void (*)(LuaObject* this_, const char*, float))
      FDecl(0x908370, SetNumber2, __thiscall void (*)(LuaObject* this_, int, float))
      FDecl(0x908810, SetObject, __thiscall void (*)(LuaObject* this_, const LuaObject&, const LuaObject&))
      FDecl(0x908760, SetObject2, __thiscall void (*)(LuaObject* this_, const char*, const LuaObject&))
      FDecl(0x9087a0, SetObject3, __thiscall void (*)(LuaObject* this_, int, const LuaObject&))
      FDecl(0x908450, SetString, __thiscall void (*)(LuaObject* this_, const char*, const char*))
      FDecl(0x9084e0, SetString2, __thiscall void (*)(LuaObject* this_, int, const char*))
      FDecl(0x9072d0, TypeError, __thiscall void (*)(LuaObject* this_, const char*))
      FDecl(0x90c1d0, Error, __cdecl int (*)(LuaObject* this_, const char*, ...))

    //protected
      FDecl(0x9074b0, SetTableHelper, __thiscall void (*)(LuaObject* this_, const char*, const TObject&))
  }

  class LuaState
  {// 0x34 bytes
    public:
      lua_State* m_state;
      void* ForMultipleThreads;
      bool m_ownState;
      LuaObject m_threadObj;
      LuaState* m_rootState;
      struct MiniLuaObject {
        LuaObject* m_next;  // only valid when in free list
        LuaObject* m_prev;  // only valid when in used list
      } m_headObject,  m_tailObject;
  };
  VALIDATE_SIZE(LuaState, 0x34)

  namespace CLuaState
  {
    //public
      enum StandardLibraries {LIB_NONE, LIB_BASE, LIB_OSIO};

      FDecl(0x90ac10, CLuaState, __thiscall LuaState (*)(LuaState* this_, enum StandardLibraries))
      FDecl(0x90a520, CLuaState2, __thiscall LuaState (*)(LuaState* this_, LuaState*))
      FDecl(0x90a5d0, CLuaState3, __thiscall LuaState (*)(LuaState* this_, int Unused))
      FDecl(0x90a600, DLuaState, __thiscall void (*)(LuaState* this_))
      FDecl(0x4579d0, GetGlobal, __thiscall LuaObject (*)(LuaState* this_, const char*))
      FDecl(0x90a690, GetGlobals, __thiscall LuaObject (*)(LuaState* this_))
      FDecl(0x90bee0, GetActiveState, __thiscall LuaState* (*)(LuaState* this_))
      FDecl(0x912d10, CheckString, __thiscall const char* (*)(LuaState* this_, int))
      FDecl(0x90bf70, ArgError, __thiscall int (*)(LuaState* this_, int, const char*))
      FDecl(0x90bef0, GetActiveCState, __thiscall lua_State* (*)(LuaState* this_))
      FDecl(0x923f20, CheckAny, __thiscall void (*)(LuaState* this_, int))

    //private
      FDecl(0x90aad0, Init, __thiscall void (*)(LuaState* this_, enum StandardLibraries))
  }

FDecl(0x90a6b0, LuaPlusH_next, bool __cdecl (*)(LuaState*, const LuaObject*, LuaObject*, LuaObject*))
FDecl(0x90ca40, lua_toboolean, bool __cdecl (*)(lua_State*, int))
FDecl(0x90c340, negindex, TObject* __cdecl (*)(lua_State*, int))
FDecl(0x90e260, luaL_prepbuffer, char* __cdecl (*)(luaL_Buffer*))
FDecl(0x90eaa0, luaL_checklstring, const char* __cdecl (*)(lua_State*, int, unsigned int*))
FDecl(0x90eb10, luaL_optlstring, const char* __cdecl (*)(lua_State*, int, const char*, unsigned int*))
FDecl(0x912680, lua_getlocal, const char* __cdecl (*)(lua_State*, const lua_Debug*, int))
FDecl(0x90d9a0, lua_getupvalue, const char* __cdecl (*)(lua_State*, int, int))
FDecl(0x90ce90, lua_pushfstring, const char* __cdecl (*)(lua_State*, const char*, ...))
FDecl(0x90ce50, lua_pushvfstring, const char* __cdecl (*)(lua_State*, const char*, char*))
FDecl(0x9126f0, lua_setlocal, const char* __cdecl (*)(lua_State*, const lua_Debug*, int))
FDecl(0x90da00, lua_setupvalue, const char* __cdecl (*)(lua_State*, int, int))
FDecl(0x90ca90, lua_tostring, const char* __cdecl (*)(lua_State*, int))
FDecl(0x90c780, lua_typename, const char* __cdecl (*)(lua_State*, int))
FDecl(0x90eb70, luaL_checknumber, float __cdecl (*)(lua_State*, int))
FDecl(0x90ebf0, luaL_optnumber, float __cdecl (*)(lua_State*, int, float))
FDecl(0x90c9f0, lua_tonumber, float __cdecl (*)(lua_State*, int))
FDecl(0x90d7e0, lua_newuserdata, RRef __cdecl (*)(lua_State*, const RRef&))
FDecl(0x90cbb0, lua_touserdata, RRef __cdecl (*)(lua_State*, int))
FDecl(0x90e900, luaL_argerror, int __cdecl (*)(lua_State*, int, const char*))
FDecl(0x90dda0, luaL_callmeta, int __cdecl (*)(lua_State*, int, const char*))
FDecl(0x90dbf0, luaL_error, int __cdecl (*)(lua_State*, const char*, ...))
FDecl(0x90dc20, luaL_findstring, int __cdecl (*)(const char*, const char* const list[]))
FDecl(0x90dd40, luaL_getmetafield, int __cdecl (*)(lua_State*, int, const char*))
FDecl(0x90e090, luaL_getn, int __cdecl (*)(lua_State*, int))
FDecl(0x90e760, luaL_loadbuffer, int __cdecl (*)(lua_State*, const char*, unsigned int, const char*))
FDecl(0x90e5d0, luaL_loadfile, int __cdecl (*)(lua_State*, const char*))
FDecl(0x90dc70, luaL_newmetatable, int __cdecl (*)(lua_State*, const char*))
FDecl(0x90e9a0, luaL_typerror, int __cdecl (*)(lua_State*, int, const char*))
FDecl(0x90c460, lua_checkstack, int __cdecl (*)(lua_State*, int))
FDecl(0x90e870, lua_dobuffer, int __cdecl (*)(lua_State*, const char*, unsigned int, const char*))
FDecl(0x90e8d0, lua_dostring, int __cdecl (*)(lua_State*, const char*))
FDecl(0x90d6c0, lua_error, int __cdecl (*)(lua_State*))
FDecl(0x90d660, lua_getgccount, int __cdecl (*)(lua_State*))
FDecl(0x90d650, lua_getgcthreshold, int __cdecl (*)(lua_State*))
FDecl(0x9125d0, lua_gethookcount, int __cdecl (*)(lua_State*))
FDecl(0x9125c0, lua_gethookmask, int __cdecl (*)(lua_State*))
FDecl(0x9132f0, lua_getinfo, int __cdecl (*)(lua_State*, const char*, lua_Debug*))
FDecl(0x90d180, lua_getmetatable, int __cdecl (*)(lua_State*, int))
FDecl(0x90ad30, lua_getn, int __cdecl (*)(lua_State*, int))
FDecl(0x9125e0, lua_getstack, int __cdecl (*)(lua_State*, int, lua_Debug*))
FDecl(0x90c590, lua_gettop, int __cdecl (*)(lua_State*))
FDecl(0x90c7a0, lua_isnumber, int __cdecl (*)(lua_State*, int))
FDecl(0x90c800, lua_isstring, int __cdecl (*)(lua_State*, int))
FDecl(0x90c980, lua_lessthan, int __cdecl (*)(lua_State*, int, int))
FDecl(0x90d5c0, lua_load, int __cdecl (*)(lua_State*, lua_Chunkreader, void*, const char*))
FDecl(0x90d6d0, lua_next, int __cdecl (*)(lua_State*, int))
FDecl(0x90d430, lua_pcall, int __cdecl (*)(lua_State*, int, int))
FDecl(0x90c890, lua_rawequal, int __cdecl (*)(lua_State*, int, int))
FDecl(0x914610, lua_resume, int __cdecl (*)(lua_State*, int))
FDecl(0x90d3b0, lua_setfenv, int __cdecl (*)(lua_State*, int))
FDecl(0x912560, lua_sethook, int __cdecl (*)(lua_State*, lua_Hook, int, int))
FDecl(0x90d340, lua_setmetatable, int __cdecl (*)(lua_State*, int))
FDecl(0x911ea0, lua_traceback, int __cdecl (*)(lua_State*, const char*, int))
FDecl(0x90c740, lua_type, int __cdecl (*)(lua_State*, int))
FDecl(0x913e40, lua_yield, int __cdecl (*)(lua_State*, int))
FDecl(0x90fd90, luaopen_base, int __cdecl (*)(lua_State*))
FDecl(0x9124c0, luaopen_debug, int __cdecl (*)(lua_State*))
FDecl(0x91a4b0, luaopen_loadlib, int __cdecl (*)(lua_State*))
FDecl(0x91a110, luaopen_math, int __cdecl (*)(lua_State*))
FDecl(0x923690, luaopen_serialize, int __cdecl (*)(lua_State*))
FDecl(0x926ef0, luaopen_string, int __cdecl (*)(lua_State*))
FDecl(0x90c530, lua_newthread, lua_State* __cdecl (*)(lua_State*))
FDecl(0x9246d0, lua_open, lua_State* __cdecl (*)())
FDecl(0x90cc50, lua_tothread, lua_State* __cdecl (*)(lua_State*, int))
FDecl(0x90cb10, lua_strlen, unsigned int __cdecl (*)(lua_State*, int))
FDecl(0x9125b0, lua_gethook, lua_Hook __cdecl (*)(lua_State*))
FDecl(0x90e2a0, luaL_addlstring, void __cdecl (*)(luaL_Buffer*, const char*, unsigned int))
FDecl(0x90e300, luaL_addstring, void __cdecl (*)(luaL_Buffer*, const char*))
FDecl(0x90e370, luaL_addvalue, void __cdecl (*)(luaL_Buffer*))
FDecl(0x90e400, luaL_buffinit, void __cdecl (*)(lua_State*, luaL_Buffer*))
FDecl(0x90ea70, luaL_checkany, void __cdecl (*)(lua_State*, int))
FDecl(0x90dd10, luaL_checkstack, void __cdecl (*)(lua_State*, int, const char*))
FDecl(0x90ea20, luaL_checktype, void __cdecl (*)(lua_State*, int, int))
FDecl(0x90dcf0, luaL_getmetatable, void __cdecl (*)(lua_State*, const char*))
FDecl(0x90de00, luaL_openlib, void __cdecl (*)(lua_State*, const char*, const luaL_reg*, int))
FDecl(0x90e330, luaL_pushresult, void __cdecl (*)(luaL_Buffer*))
FDecl(0x90dfb0, luaL_setn, void __cdecl (*)(lua_State*, int, int))
FDecl(0x90db80, luaL_where, void __cdecl (*)(lua_State*, int))
FDecl(0x90d400, lua_call, void __cdecl (*)(lua_State*, int, int))
FDecl(0x9243e0, lua_close, void __cdecl (*)(lua_State*))
FDecl(0x90d740, lua_concat, void __cdecl (*)(lua_State*, int))
FDecl(0x90d1f0, lua_getfenv, void __cdecl (*)(lua_State*, int))
FDecl(0x90d000, lua_gettable, void __cdecl (*)(lua_State*, int))
FDecl(0x90c640, lua_insert, void __cdecl (*)(lua_State*, int))
FDecl(0x90d110, lua_newtable, void __cdecl (*)(lua_State*))
FDecl(0x90cf80, lua_pushboolean, void __cdecl (*)(lua_State*, int))
FDecl(0x90ced0, lua_pushcclosure, void __cdecl (*)(lua_State*, lua_CFunction, int))
FDecl(0x90cfc0, lua_pushlightuserdata, void __cdecl (*)(lua_State*, void*))
FDecl(0x90cd80, lua_pushlstring, void __cdecl (*)(lua_State*, const char*, unsigned int))
FDecl(0x90cd00, lua_pushnil, void __cdecl (*)(lua_State*))
FDecl(0x90cd40, lua_pushnumber, void __cdecl (*)(lua_State*, float))
FDecl(0x90cdf0, lua_pushstring, void __cdecl (*)(lua_State*, const char*))
FDecl(0x90c6e0, lua_pushvalue, void __cdecl (*)(lua_State*, int))
FDecl(0x90d050, lua_rawget, void __cdecl (*)(lua_State*, int))
FDecl(0x90d0a0, lua_rawgeti, void __cdecl (*)(lua_State*, int, int))
FDecl(0x90d2a0, lua_rawset, void __cdecl (*)(lua_State*, int))
FDecl(0x90d2f0, lua_rawseti, void __cdecl (*)(lua_State*, int, int))
FDecl(0x90c5f0, lua_remove, void __cdecl (*)(lua_State*, int))
FDecl(0x90c690, lua_replace, void __cdecl (*)(lua_State*, int))
FDecl(0x90ad00, lua_setdefaultmetatable, void __cdecl (*)(lua_State*, int))
FDecl(0x90d670, lua_setgcthreshold, void __cdecl (*)(lua_State*, int))
FDecl(0x924060, lua_setglobaluserdata, void __cdecl (*)(lua_State*, void*))
FDecl(0x9240b0, lua_setstateuserdata, void __cdecl (*)(lua_State*, void*))
FDecl(0x90d260, lua_settable, void __cdecl (*)(lua_State*, int))
FDecl(0x90c5a0, lua_settop, void __cdecl (*)(lua_State*, int))
FDecl(0x924080, lua_setusergcfunction, void __cdecl (*)(lua_State*, userGCFunction))
FDecl(0x90c4c0, lua_xmove, void __cdecl (*)(lua_State*, lua_State*, int))
FDecl(0x90cc90, lua_topointer, void const* __cdecl (*)(lua_State*, int))
FDecl(0x924050, lua_getglobaluserdata, void* __cdecl (*)(lua_State*))
FDecl(0x9240a0, lua_getstateuserdata, void* __cdecl (*)(lua_State*))
FDecl(0x90cc10, lua_tolightuserdata, void* __cdecl (*)(lua_State*, int))