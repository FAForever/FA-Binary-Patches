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
      LuaObject() {
        ((__thiscall LuaObject* (*)(LuaObject*))0x9072a0)(this);
      }
      LuaObject(LuaState* state) {
        ((__thiscall LuaObject* (*)(LuaObject*, LuaState*))0x908970)(this, state);
      }
      LuaObject(LuaState* state, int index) {
        ((__thiscall LuaObject* (*)(LuaObject*, LuaState*, int))0x9089c0)(this, state, index);
      }
      LuaObject(const LuaObject* obj) {
        ((__thiscall LuaObject* (*)(LuaObject*, const LuaObject*))0x908a40)(this, obj);
      }
      LuaObject(const LuaStackObject* stack) {
        ((__thiscall LuaObject* (*)(LuaObject*, const LuaStackObject*))0x908a70)(this, stack);
      }
      LuaObject(LuaState* state, const TObject* obj) {
        ((__thiscall LuaObject* (*)(LuaObject*, LuaState*, const TObject*))0x9089f0)(this, state, obj);
      }
      ~LuaObject() {
        ((__thiscall void (*)(LuaObject*))0x9075d0)(this);
      }
      LuaObject* operator=(const LuaObject* obj) {
        return ((__thiscall LuaObject* (*)(LuaObject*, const LuaObject*))0x908ab0)(this, obj);
      }
      LuaObject* operator=(const LuaStackObject* stack) {
        return ((__thiscall LuaObject* (*)(LuaObject*, const LuaStackObject*))0x908b00)(this, stack);
      }
      LuaObject operator[](int key) {
        LuaObject out;
        ((__thiscall void (*)(LuaObject*, LuaObject*, int))0x9091e0)(this, &out, key);
        return out;
      }
      LuaObject operator[](const char* key) {
        LuaObject out;
        ((__thiscall void (*)(LuaObject*, LuaObject*, const char*))0x908f60)(this, &out, key);
        return out;
      }
      bool GetBoolean() {
        return ((__thiscall bool (*)(LuaObject*))0x907c90)(this);
      }
      bool IsBoolean() {
        return ((__thiscall bool (*)(LuaObject*))0x9078d0)(this);
      }
      bool IsConvertibleToString() {
        return ((__thiscall bool (*)(LuaObject*))0x9077c0)(this);
      }
      bool IsFunction() {
        return ((__thiscall bool (*)(LuaObject*))0x907810)(this);
      }
      bool IsInteger() {
        return ((__thiscall bool (*)(LuaObject*))0x907350)(this);
      }
      bool IsNil() {
        return ((__thiscall bool (*)(LuaObject*))0x9072f0)(this);
      }
      bool IsNumber() {
        return ((__thiscall bool (*)(LuaObject*))0x907360)(this);
      }
      bool IsString() {
        return ((__thiscall bool (*)(LuaObject*))0x907370)(this);
      }
      bool IsTable() {
        return ((__thiscall bool (*)(LuaObject*))0x907310)(this);
      }
      bool IsUserData() {
        return ((__thiscall bool (*)(LuaObject*))0x907320)(this);
      }
      void Clone(LuaObject* out) {
        ((__thiscall void (*)(LuaObject*,  LuaObject*))0x90a180)(this, out);
      }
      void CreateTable(LuaObject* out, const char* key, int narray, int lnhash) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, const char*, int, int))0x908c10)(this, out, key, narray, lnhash);
      }
      void CreateTable(LuaObject* out, int key, int narray, int lnhash) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, int, int, int))0x908ca0)(this, out, key, narray, lnhash);
      }
      void GetByIndex(LuaObject* out, int index) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, int))0x908df0)(this, out, index);
      }
      void GetByName(LuaObject* out, const char* name) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, const char*))0x90a160)(this, out, name);
      }
      void GetByObject(LuaObject* out, const LuaObject* obj) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, const LuaObject*))0x908e70)(this, out, obj);
      }
      void GetMetaTable(LuaObject* out) {
        ((__thiscall void (*)(LuaObject*, LuaObject*))0x908ba0)(this, out);
      }
      void Lookup(LuaObject* out, const char* key) {
        ((__thiscall void (*)(LuaObject*, LuaObject*, const char*))0x9093b0)(this, out, key);
      }
      void PushStack(LuaStackObject* out, LuaState* state) {
        ((__thiscall void (*)(LuaObject*, LuaStackObject*, LuaState*))0x907d80)(this, out, state);
      }
      void PushStack(lua_State* L) {
        ((__thiscall void (*)(LuaObject*, lua_State*))0x907d10)(this, L);
      }
      LuaState* GetActiveState() {
        return ((__thiscall LuaState* (*)(LuaObject*))0x9072b0)(this);
      }
      const char* GetString() {
        return ((__thiscall const char* (*)(LuaObject*))0x907a90)(this);
      }
      const char* ToString() {
        return ((__thiscall const char* (*)(LuaObject*))0x9073e0)(this);
      }
      const char* TypeName() {
        return ((__thiscall const char* (*)(LuaObject*))0x908b50)(this);
      }
      lua_Number GetNumber() {
        return ((__thiscall float (*)(LuaObject*))0x907970)(this);
      }
      lua_Number GetDouble() { //Same as GetNumber
        return ((__thiscall float (*)(LuaObject*))0x907a30)(this);
      }
      lua_Number ToNumber() {
        return ((__thiscall float (*)(LuaObject*))0x9073b0)(this);
      }
      void AssignNewUserData(RRef* out, LuaState* state, const RRef* rRef) {
        ((__thiscall void (*)(LuaObject*, RRef*, LuaState*, const RRef*))0x909840)(this, out, state, rRef);
      }
      void AssignNewUserData(RRef* out, LuaState* state, const RType* rType) {
        ((__thiscall void (*)(LuaObject*, RRef*, LuaState*, const RType*))0x9097d0)(this, out, state, rType);
      }
      void GetUserData(RRef* out) {
        ((__thiscall void (*)(LuaObject*, RRef*))0x907bc0)(this, out);
      }
      int GetCount() {
        return ((__thiscall int (*)(LuaObject*))0x907f50)(this);
      }
      int GetInteger() {
        return ((__thiscall int (*)(LuaObject*))0x907910)(this);
      }
      int GetN() {
        return ((__thiscall int (*)(LuaObject*))0x907e50)(this);
      }
      int GetTableCount() {
        return ((__thiscall int (*)(LuaObject*))0x90a410)(this);
      }
      int IsPassed() {
        return ((__thiscall int (*)(LuaObject*))0x907440)(this);
      }
      int Type() {
        return ((__thiscall int (*)(LuaObject*))0x9076d0)(this);
      }
      lua_State* GetActiveCState() {
        return ((__thiscall lua_State* (*)(LuaObject*))0x9072c0)(this);
      }
      void AssignBoolean(LuaState* state, bool value) {
        ((__thiscall void (*)(LuaObject*, LuaState*, bool))0x909600)(this, state, value);
      }
      void AssignInteger(LuaState* state, int value) {
        ((__thiscall void (*)(LuaObject*, LuaState*, int))0x909650)(this, state, value);
      }
      void AssignNewTable(LuaState* state, int narray, int lnhash) {
        ((__thiscall void (*)(LuaObject*, LuaState*, int, int))0x909940)(this, state, narray, lnhash);
      }
      void AssignNil(LuaState* state) {
        ((__thiscall void (*)(LuaObject*, LuaState*))0x9095c0)(this, state);
      }
      void AssignNumber(LuaState* state, float value) {
        ((__thiscall void (*)(LuaObject*, LuaState*, float))0x9096a0)(this, state, value);
      }
      void AssignString(LuaState* state, const char* value) {
        ((__thiscall void (*)(LuaObject*, LuaState*, const char*))0x909750)(this, state, value);
      }
      void AssignTObject(LuaState* state, const TObject* value) {
        ((__thiscall void (*)(LuaObject*, LuaState*, const TObject*))0x9099b0)(this, state, value);
      }
      void AssignThread(LuaState* state) {
        ((__thiscall void (*)(LuaObject*, LuaState*))0x9096f0)(this, state);
      }
      void Insert(LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, LuaObject*))0x909af0)(this, value);
      }
      void Insert(int key, LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, int, LuaObject*))0x909ce0)(this, key, value);
      }
      void Register(const char* name, lua_CFunction func, int nupvalues) {
        ((__thiscall void (*)(LuaObject*, const char*, lua_CFunction, int))0x907630)(this, name, func, nupvalues);
      }
      void Reset() {
        ((__thiscall void (*)(LuaObject*))0x9075f0)(this);
      }
      void SetBoolean(const char* key, bool value) {
        ((__thiscall void (*)(LuaObject*, const char*, bool))0x9080c0)(this, key, value);
      }
      void SetInteger(const char* key, int value) {
        ((__thiscall void (*)(LuaObject*, const char*, int))0x9081f0)(this, key, value);
      }
      void SetInteger(int key, int value) {
        ((__thiscall void (*)(LuaObject*, int, int))0x908240)(this, key, value);
      }
      void SetMetaTable(const LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, const LuaObject*))0x907e00)(this, value);
      }
      void SetN(int n) {
        ((__thiscall void (*)(LuaObject*, int))0x907ed0)(this, n);
      }
      void SetNil(const char* key) {
        ((__thiscall void (*)(LuaObject*, const char*))0x907fa0)(this, key);
      }
      void SetNil(int key) {
        ((__thiscall void (*)(LuaObject*, int))0x907ff0)(this, key);
      }
      void SetNumber(const char* key, float value) {
        ((__thiscall void (*)(LuaObject*, const char*, float))0x908320)(this, key, value);
      }
      void SetNumber(int key, float value) {
        ((__thiscall void (*)(LuaObject*, int, float))0x908370)(this, key, value);
      }
      void SetObject(const LuaObject* key, const LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, const LuaObject*, const LuaObject*))0x908810)(this, key, value);
      }
      void SetObject(const char* key, const LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, const char*, const LuaObject*))0x908760)(this, key, value);
      }
      void SetObject(int key, const LuaObject* value) {
        ((__thiscall void (*)(LuaObject*, int, const LuaObject*))0x9087a0)(this, key, value);
      }
      void SetString(const char* key, const char* value) {
        ((__thiscall void (*)(LuaObject*, const char*, const char*))0x908450)(this, key, value);
      }
      void SetString(int key, const char* value) {
        ((__thiscall void (*)(LuaObject*, int, const char*))0x9084e0)(this, key, value);
      }
      void TypeError(const char* msg) {
        ((__thiscall void (*)(LuaObject*, const char*))0x9072d0)(this, msg);
      }
      int Error(const char* fmt, ...) {
        return ((__cdecl int (*)(LuaObject*, const char*, ...))0x90c1d0)(this, fmt, (uintptr_t*)fmt + 1);
      }

    //private
      void AddToUsedList(LuaState* state) {
        ((__thiscall void (*)(LuaObject*, LuaState*))0x908890)(this, state);
      }
      void AddToUsedList(LuaState* state, const TObject* obj) {
        ((__thiscall void (*)(LuaObject*, LuaState*, const TObject*))0x9088e0)(this, state, obj);
      }
      void SetTableHelper(const char* key, const TObject* value) {
        ((__thiscall void (*)(LuaObject*, const char*, const TObject*))0x9074b0)(this, key, value);
      }

      LuaObject* m_next;
      LuaObject* m_prev;
      LuaState* m_state;
      TObject m_object;
  };
  VALIDATE_SIZE(LuaObject, 0x14)

  class LuaState
  {// 0x34 bytes
    public:
      enum StandardLibraries {LIB_NONE, LIB_BASE, LIB_OSIO};

      LuaState(enum StandardLibraries libs) {
        ((__thiscall LuaState* (*)(LuaState*, enum StandardLibraries))0x90ac10)(this, libs);
      }
      LuaState(LuaState* parentState) {
        ((__thiscall LuaState* (*)(LuaState*, LuaState*))0x90a520)(this, parentState);
      }
      LuaState(int Unused) {
        ((__thiscall LuaState* (*)(LuaState*, int))0x90a5d0)(this, Unused);
      }
      ~LuaState() {
        ((__thiscall void (*)(LuaState*))0x90a600)(this);
      }
      void GetGlobal(LuaObject* out, const char* key) {
        ((__thiscall void (*)(LuaState*, LuaObject*, const char*))0x4579d0)(this, out, key);
      }
      void GetGlobals(LuaObject* out) {
        ((__thiscall void (*)(LuaState*, LuaObject*))0x90a690)(this, out);
      }
      LuaState* GetActiveState() {
        return ((__thiscall LuaState* (*)(LuaState*))0x90bee0)(this);
      }
      const char* CheckString(int narg) {
        return ((__thiscall const char* (*)(LuaState*, int))0x912d10)(this, narg);
      }
      int ArgError(int narg, const char* msg) {
        return ((__thiscall int (*)(LuaState*, int, const char*))0x90bf70)(this, narg, msg);
      }
      lua_State* GetActiveCState() {
        return ((__thiscall lua_State* (*)(LuaState*))0x90bef0)(this);
      }
      void CheckAny(int narg) {
        ((__thiscall void (*)(LuaState*, int))0x923f20)(this, narg);
      }

    //private
      void Init(enum StandardLibraries libs) {
        ((__thiscall void (*)(LuaState*, enum StandardLibraries))0x90aad0)(this, libs);
      }

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