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
#define LUA_CFUNCTION		6
#define LUA_TFUNCTION		7
#define LUA_TUSERDATA		8
#define LUA_TTHREAD		9
#define LUA_TPROTO		10
#define LUA_TUPVALUE		11

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

#define lua_isnoneornil(L,n)    (lua_type(L,n) <= 0)
#define lua_isnone(L,n)         (lua_type(L,n) == LUA_TNONE)
#define lua_isnil(L,n)          (lua_type(L,n) == LUA_TNIL)
#define lua_isboolean(L,n)      (lua_type(L,n) == LUA_TBOOLEAN)
#define lua_islightuserdata(L,n)(lua_type(L,n) == LUA_TLIGHTUSERDATA)
#define lua_istable(L,n)        (lua_type(L,n) == LUA_TTABLE)
#define lua_iscfunction(L,n)    (lua_type(L,n) == LUA_CFUNCTION)
#define lua_isfunction(L,n)     (lua_type(L,n) == LUA_TFUNCTION)
#define lua_isuserdata(L,n)     (lua_type(L,n) == LUA_TUSERDATA)
#define lua_isthread(L,n)       (lua_type(L,n) == LUA_TTHREAD)
#define lua_isproto(L,n)        (lua_type(L,n) == LUA_TPROTO)
#define lua_isupvalue(L,n)      (lua_type(L,n) == LUA_TUPVALUE)

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
      void __LuaObject() asm("0x9072a0");
      void __LuaObject(LuaState* state) asm("0x908970");
      void __LuaObject(LuaState* state, int index) asm("0x9089c0");
      void __LuaObject(const LuaObject* obj) asm("0x908a40");
      void __LuaObject(const LuaStackObject* stack) asm("0x908a70");
      void __LuaObject(LuaState* state, const TObject* obj) asm("0x9089f0");
      void __DLuaObject() asm("0x9075d0");
      void __Index(LuaObject *out, int key) asm("0x9091e0");
      void __Index(LuaObject *out, const char* key) asm("0x908f60");

    public:
      LuaObject() {__LuaObject();}
      LuaObject(LuaState* state) {__LuaObject(state);}
      LuaObject(LuaState* state, int index) {__LuaObject(state, index);}
      LuaObject(const LuaObject* obj) {__LuaObject(obj);}
      LuaObject(const LuaStackObject* stack) {__LuaObject(stack);}
      LuaObject(LuaState* state, const TObject* obj) {__LuaObject(state, obj);}
      ~LuaObject() {__DLuaObject();}

      LuaObject* operator=(const LuaObject* obj) asm("0x908ab0");
      LuaObject* operator=(const LuaStackObject* stack) asm("0x908b00");

      LuaObject operator[](int key) {
        LuaObject out; __Index(&out, key); return out;
      }
      LuaObject operator[](const char* key) {
        LuaObject out; __Index(&out, key); return out;
      }

      bool GetBoolean() asm("0x907c90");
      bool IsBoolean() asm("0x9078d0");
      bool IsConvertibleToString() asm("0x9077c0");
      bool IsFunction() asm("0x907810");
      bool IsInteger() asm("0x907350");
      bool IsNil() asm("0x9072f0");
      bool IsNumber() asm("0x907360");
      bool IsString() asm("0x907370");
      bool IsTable() asm("0x907310");
      bool IsUserData() asm("0x907320");
      void Clone(LuaObject* out) asm("0x90a180");
      void CreateTable(LuaObject* out, const char* key, int narray, int lnhash) asm("0x908c10");
      void CreateTable(LuaObject* out, int key, int narray, int lnhash) asm("0x908ca0");
      void GetByIndex(LuaObject* out, int index) asm("0x908df0");
      void GetByName(LuaObject* out, const char* name) asm("0x90a160");
      void GetByObject(LuaObject* out, const LuaObject* obj) asm("0x908e70");
      void GetMetaTable(LuaObject* out) asm("0x908ba0");
      void Lookup(LuaObject* out, const char* key) asm("0x9093b0");
      void PushStack(LuaStackObject* out, LuaState* state) asm("0x907d80");
      void PushStack(lua_State* L) asm("0x907d10");
      LuaState* GetActiveState() asm("0x9072b0");
      const char* GetString() asm("0x907a90");
      const char* ToString() asm("0x9073e0");
      const char* TypeName() asm("0x908b50");
      lua_Number GetNumber() asm("0x907970");
      lua_Number GetDouble() asm("0x907a30");
      lua_Number ToNumber() asm("0x9073b0");
      void AssignNewUserData(RRef* out, LuaState* state, const RRef* rRef) asm("0x909840");
      void AssignNewUserData(RRef* out, LuaState* state, const RType* rType) asm("0x9097d0");
      void GetUserData(RRef* out) asm("0x907bc0");
      int GetCount() asm("0x907f50");
      int GetInteger() asm("0x907910");
      int GetN() asm("0x907e50");
      int GetTableCount() asm("0x90a410");
      int IsPassed() asm("0x907440");
      int Type() asm("0x9076d0");
      lua_State* GetActiveCState() asm("0x9072c0");
      void AssignBoolean(LuaState* state, bool value) asm("0x909600");
      void AssignInteger(LuaState* state, int value) asm("0x909650");
      void AssignNewTable(LuaState* state, int narray, int lnhash) asm("0x909940");
      void AssignNil(LuaState* state) asm("0x9095c0");
      void AssignNumber(LuaState* state, float value) asm("0x9096a0");
      void AssignString(LuaState* state, const char* value) asm("0x909750");
      void AssignTObject(LuaState* state, const TObject* value) asm("0x9099b0");
      void AssignThread(LuaState* state) asm("0x9096f0");
      void Insert(LuaObject* value) asm("0x909af0");
      void Insert(int key, LuaObject* value) asm("0x909ce0");
      void Register(const char* name, lua_CFunction func, int nupvalues) asm("0x907630");
      void Reset() asm("0x9075f0");
      void SetBoolean(const char* key, bool value) asm("0x9080c0");
      void SetInteger(const char* key, int value) asm("0x9081f0");
      void SetInteger(int key, int value) asm("0x908240");
      void SetMetaTable(const LuaObject* value) asm("0x907e00");
      void SetN(int n) asm("0x907ed0");
      void SetNil(const char* key) asm("0x907fa0");
      void SetNil(int key) asm("0x907ff0");
      void SetNumber(const char* key, float value) asm("0x908320");
      void SetNumber(int key, float value) asm("0x908370");
      void SetObject(const LuaObject* key, const LuaObject* value) asm("0x908810");
      void SetObject(const char* key, const LuaObject* value) asm("0x908760");
      void SetObject(int key, const LuaObject* value) asm("0x9087a0");
      void SetString(const char* key, const char* value) asm("0x908450");
      void SetString(int key, const char* value) asm("0x9084e0");
      void TypeError(const char* msg) asm("0x9072d0");

    //private
      void AddToUsedList(LuaState* state) asm("0x908890");
      void AddToUsedList(LuaState* state, const TObject* obj) asm("0x9088e0");
      void SetTableHelper(const char* key, const TObject* value) asm("0x9074b0");

      LuaObject* m_next;
      LuaObject* m_prev;
      LuaState* m_state;
      TObject m_object;
  };
  VALIDATE_SIZE(LuaObject, 0x14)

  enum StandardLibraries {LIB_NONE, LIB_BASE, LIB_OSIO};
  class LuaState
  {// 0x34 bytes
      void __LuaState(StandardLibraries libs) asm("0x90ac10");
      void __LuaState(LuaState* parentState) asm("0x90a520");
      void __LuaState(int Unused) asm("0x90a5d0");
      void __DLuaState() asm("0x90a600");
    public:
      LuaState(StandardLibraries libs) {__LuaState(libs);};
      LuaState(LuaState* parentState) {__LuaState(parentState);};
      LuaState(int Unused) {__LuaState(Unused);};
      ~LuaState() {__DLuaState();};
      void GetGlobal(LuaObject* out, const char* key) asm("0x4579d0");
      void GetGlobals(LuaObject* out) asm("0x90a690");
      LuaState* GetActiveState() asm("0x90bee0");
      const char* CheckString(int narg) asm("0x912d10");
      int ArgError(int narg, const char* msg) asm("0x90bf70");
      __cdecl int Error(const char* fmt, ...) asm("0x90c1d0");
      lua_State* GetActiveCState() asm("0x90bef0");
      void CheckAny(int narg) asm("0x923f20");

    //private
      void Init(enum StandardLibraries libs) asm("0x90aad0");

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

bool LuaPlusH_next(LuaState*, const LuaObject*, LuaObject*, LuaObject*) asm("0x90a6b0");
bool lua_toboolean(lua_State*, int) asm("0x90ca40");
TObject* negindex(lua_State*, int) asm("0x90c340");
char* luaL_prepbuffer(luaL_Buffer*) asm("0x90e260");
const char* luaL_checklstring(lua_State*, int, unsigned int*) asm("0x90eaa0");
const char* luaL_optlstring(lua_State*, int, const char*, unsigned int*) asm("0x90eb10");
const char* lua_getlocal(lua_State*, const lua_Debug*, int) asm("0x912680");
const char* lua_getupvalue(lua_State*, int, int) asm("0x90d9a0");
const char* lua_pushfstring(lua_State*, const char*, ...) asm("0x90ce90");
const char* lua_pushvfstring(lua_State*, const char*, char*) asm("0x90ce50");
const char* lua_setlocal(lua_State*, const lua_Debug*, int) asm("0x9126f0");
const char* lua_setupvalue(lua_State*, int, int) asm("0x90da00");
const char* lua_tostring(lua_State*, int) asm("0x90ca90");
const char* lua_typename(lua_State*, int) asm("0x90c780");
float luaL_checknumber(lua_State*, int) asm("0x90eb70");
float luaL_optnumber(lua_State*, int, float) asm("0x90ebf0");
float lua_tonumber(lua_State*, int) asm("0x90c9f0");
void lua_newuserdata(RRef *ret, lua_State*, const RType*) asm("0x90d7e0");
void lua_touserdata(RRef *ret, lua_State*, int) asm("0x90cbb0");
int luaL_argerror(lua_State*, int, const char*) asm("0x90e900");
int luaL_callmeta(lua_State*, int, const char*) asm("0x90dda0");
int luaL_error(lua_State*, const char*, ...) asm("0x90dbf0");
int luaL_findstring(const char*, const char* const list[]) asm("0x90dc20");
int luaL_getmetafield(lua_State*, int, const char*) asm("0x90dd40");
int luaL_getn(lua_State*, int) asm("0x90e090");
int luaL_loadbuffer(lua_State*, const char*, unsigned int, const char*) asm("0x90e760");
int luaL_loadfile(lua_State*, const char*) asm("0x90e5d0");
int luaL_newmetatable(lua_State*, const char*) asm("0x90dc70");
int luaL_typerror(lua_State*, int, const char*) asm("0x90e9a0");
int lua_checkstack(lua_State*, int) asm("0x90c460");
int lua_dobuffer(lua_State*, const char*, unsigned int, const char*) asm("0x90e870");
int lua_dostring(lua_State*, const char*) asm("0x90e8d0");
int lua_error(lua_State*) asm("0x90d6c0");
int lua_getgccount(lua_State*) asm("0x90d660");
int lua_getgcthreshold(lua_State*) asm("0x90d650");
int lua_gethookcount(lua_State*) asm("0x9125d0");
int lua_gethookmask(lua_State*) asm("0x9125c0");
int lua_getinfo(lua_State*, const char*, lua_Debug*) asm("0x9132f0");
int lua_getmetatable(lua_State*, int) asm("0x90d180");
int lua_getn(lua_State*, int) asm("0x90ad30");
int lua_getstack(lua_State*, int, lua_Debug*) asm("0x9125e0");
int lua_gettop(lua_State*) asm("0x90c590");
int lua_isnumber(lua_State*, int) asm("0x90c7a0");
int lua_isstring(lua_State*, int) asm("0x90c800");
int lua_lessthan(lua_State*, int, int) asm("0x90c980");
int lua_load(lua_State*, lua_Chunkreader, void*, const char*) asm("0x90d5c0");
int lua_next(lua_State*, int) asm("0x90d6d0");
int lua_pcall(lua_State*, int, int) asm("0x90d430");
int lua_rawequal(lua_State*, int, int) asm("0x90c890");
int lua_resume(lua_State*, int) asm("0x914610");
int lua_setfenv(lua_State*, int) asm("0x90d3b0");
int lua_sethook(lua_State*, lua_Hook, int, int) asm("0x912560");
int lua_setmetatable(lua_State*, int) asm("0x90d340");
int lua_traceback(lua_State*, const char*, int) asm("0x911ea0");
int lua_type(lua_State*, int) asm("0x90c740");
int lua_yield(lua_State*, int) asm("0x913e40");
int luaopen_base(lua_State*) asm("0x90fd90");
int luaopen_debug(lua_State*) asm("0x9124c0");
int luaopen_loadlib(lua_State*) asm("0x91a4b0");
int luaopen_math(lua_State*) asm("0x91a110");
int luaopen_serialize(lua_State*) asm("0x923690");
int luaopen_string(lua_State*) asm("0x926ef0");
lua_State* lua_newthread(lua_State*) asm("0x90c530");
lua_State* lua_open() asm("0x9246d0");
lua_State* lua_tothread(lua_State*, int) asm("0x90cc50");
unsigned int lua_strlen(lua_State*, int) asm("0x90cb10");
lua_Hook lua_gethook(lua_State*) asm("0x9125b0");
void luaL_addlstring(luaL_Buffer*, const char*, unsigned int) asm("0x90e2a0");
void luaL_addstring(luaL_Buffer*, const char*) asm("0x90e300");
void luaL_addvalue(luaL_Buffer*) asm("0x90e370");
void luaL_buffinit(lua_State*, luaL_Buffer*) asm("0x90e400");
void luaL_checkany(lua_State*, int) asm("0x90ea70");
void luaL_checkstack(lua_State*, int, const char*) asm("0x90dd10");
void luaL_checktype(lua_State*, int, int) asm("0x90ea20");
void luaL_getmetatable(lua_State*, const char*) asm("0x90dcf0");
void luaL_openlib(lua_State*, const char*, const luaL_reg*, int) asm("0x90de00");
void luaL_pushresult(luaL_Buffer*) asm("0x90e330");
void luaL_setn(lua_State*, int, int) asm("0x90dfb0");
void luaL_where(lua_State*, int) asm("0x90db80");
void lua_call(lua_State*, int, int) asm("0x90d400");
void lua_close(lua_State*) asm("0x9243e0");
void lua_concat(lua_State*, int) asm("0x90d740");
void lua_getfenv(lua_State*, int) asm("0x90d1f0");
void lua_gettable(lua_State*, int) asm("0x90d000");
void lua_insert(lua_State*, int) asm("0x90c640");
void lua_newtable(lua_State*) asm("0x90d110");
void lua_pushboolean(lua_State*, int) asm("0x90cf80");
void lua_pushcclosure(lua_State*, lua_CFunction, int) asm("0x90ced0");
void lua_pushlightuserdata(lua_State*, void*) asm("0x90cfc0");
void lua_pushlstring(lua_State*, const char*, unsigned int) asm("0x90cd80");
void lua_pushnil(lua_State*) asm("0x90cd00");
void lua_pushnumber(lua_State*, float) asm("0x90cd40");
void lua_pushstring(lua_State*, const char*) asm("0x90cdf0");
void lua_pushvalue(lua_State*, int) asm("0x90c6e0");
void lua_rawget(lua_State*, int) asm("0x90d050");
void lua_rawgeti(lua_State*, int, int) asm("0x90d0a0");
void lua_rawset(lua_State*, int) asm("0x90d2a0");
void lua_rawseti(lua_State*, int, int) asm("0x90d2f0");
void lua_remove(lua_State*, int) asm("0x90c5f0");
void lua_replace(lua_State*, int) asm("0x90c690");
void lua_setdefaultmetatable(lua_State*, int) asm("0x90ad00");
void lua_setgcthreshold(lua_State*, int) asm("0x90d670");
void lua_setglobaluserdata(lua_State*, void*) asm("0x924060");
void lua_setstateuserdata(lua_State*, void*) asm("0x9240b0");
void lua_settable(lua_State*, int) asm("0x90d260");
void lua_settop(lua_State*, int) asm("0x90c5a0");
void lua_setusergcfunction(lua_State*, userGCFunction) asm("0x924080");
void lua_xmove(lua_State*, lua_State*, int) asm("0x90c4c0");
void const* lua_topointer(lua_State*, int) asm("0x90cc90");
void* lua_getglobaluserdata(lua_State*) asm("0x924050");
void* lua_getstateuserdata(lua_State*) asm("0x9240a0");
void* lua_tolightuserdata(lua_State*, int) asm("0x90cc10");