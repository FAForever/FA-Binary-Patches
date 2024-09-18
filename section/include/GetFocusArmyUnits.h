#include "moho.h"

namespace Moho {
template <typename T> struct BaseVector {
  T *begin;
  T *end;
  T *capacity_end;
};

template <typename T, size_t N> struct InlinedVector : BaseVector<T> {
  T *inlined;
  T inlined_items[N];

  InlinedVector() {
    this->begin = inlined_items;
    this->end = inlined_items;
    this->capacity_end = inlined_items + N;
    inlined = inlined_items;
  }

  ~InlinedVector() {
    if (this->begin != inlined) {
      free(this->begin);
      this->begin = inlined_items;
      this->end = this->begin;
      this->capacity_end = inlined_items + N;
    }
  }
};

enum map_node_color : char { // XREF: map_node/r
  BLACK = 0x0,
  RED = 0x1,
};

template <typename K, typename V>
struct map_node // sizeof=0x1A
{
  map_node<K, V> *left;
  map_node<K, V> *parent;
  map_node<K, V> *right;
  K *key;
  V *value;
  void *field_14;
  map_node_color color;
  bool is_leaf;
};

template <typename K, typename V> struct map {
  void *unk;
  map_node<K, V> *root;
  int field_8;
};

template <typename T> struct FastVector {
  FastVector<T> *self;
  BaseVector<T> data;
};

// by Hdt80bro

struct struct_BuildTemplate {

  Vector3f pos;
  int buildOrder;
  string unitId;
};
namespace VisionDB {

struct Pool // sizeof=0x1C
{
  void *vtable;
  int v1;
  int vec1;
  int v3;
  int v4;
  int vec2;
  int v6;
};
} // namespace VisionDB

struct struct_session_res3 // sizeof=0x84
{
  int v0;
  void **data;
  void **data_end;
  int v3;
  int v4;
  int v5;
  int v6;
  int v7;
  int v8;
  int v9;
  int v10;
  int v11;
  int v12;
  int v13;
  int v14;
  int v15;
  int v16;
  int v17;
  int v18;
  int v19;
  int v20;
  int v21;
  int v22;
  int v23;
  int v24;
  int v25;
  int v26;
  int v27;
  int width1;
  int width2;
  int v30;
  int v31;
  int size;
};

struct CWldSession // sizeof=0x508
{
  bool v0a;
  bool v0b;
  bool v0c;
  bool v0d;
  int stiMap;
  int v2;
  int v3;
  LuaState *state;
  /* Moho::CTaskStage*/ void *curThread;
  /* Moho::RRuleGameRulesImpl*/ void *gamerules;
  /* Moho::CWldMap*/ void *wldmap;
  int canRestartvar;
  int v9;
  string mapname;
  int v17;
  /*std::vector*/ void *vec1;
  int v19;
  struct_session_res3 v20;
  int v53;
  int v54;
  int v55;
  int v56;
  /*std::vector*/ void *vec2;
  int v58;
  int v59;
  InlinedVector<struct_BuildTemplate, 16> buildTemplate;
  float templateArg1;
  float templateArg2;
  int visionDB;
  VisionDB::Pool visionDBPool;
  int v250;
  FastVector<UserArmy *> userArmies;
  /*struct_session_res1*/ void *res1;
  /*struct_session_res2*/ void *res2;
  /*Moho::WeakPtr_UICommandGraph*/ struct {
    void *a[2];
  } uiCommandGraph;
  int v259;
  int v260;
  int v261;
  int v262;
  int v263;
  int v264;
  int v265;
  int v266;
  int v267;
  /*std::vector*/ void *vec3;
  int v269;
  int v270;
  /*std::vector*/ void *vec4;
  int v272;
  LuaObject scenarioInfo;
  int gameTick;
  int v279;
  float timingElement1;
  bool v281a;
  bool v281b;
  bool v281c;
  bool v281d;
  int v282;
  bool replayIsPaused;
  bool v284b;
  bool v284c;
  bool v284d;
  FastVector</*SSTICommandSource */ void *> commandSources;
  int localCmdSrc;
  bool isReplay;
  bool isBeingRecorded;
  bool isMultiplayer;
  bool isObservingAllowed;
  int focusArmy;
  bool gameIsOver;
  bool v291b;
  bool v291c;
  bool v291d;
  int v292;
  int v293;
  int v294;
  int v295;
  map<void *, void *> selectedUnits;
  int v299;
  bool v300a;
  bool v300b;
  bool v300c;
  bool v300d;
  Vector3f mouseWorldPos;
  int v304;
  int v305;
  int isDragger;
  Vector2f mouseScreenPos;
  bool cheatsEnabled;
  bool v309b;
  bool v309c;
  bool v309d;
  int v310;
  int v311;
  int v312;
  int v313;
  bool renderOverlayEconomy;
  bool teamColorMode;
  bool v314c;
  bool v314d;
  int v315;
  int v316;
  int v317;
  int v318;
  int v319;
  int v320;
  int v321;
};

using ::UserEntity;
using ::UserUnit;
struct UserEntityVTable {
  void *(__thiscall *dtr)(void *_this, char);
  void(__stdcall *Tick)(int);
  void *IsUserUnit1;
  UserUnit *(__thiscall *IsUserUnit2)(UserEntity *);
  void *GetUniformScale;
  void *GetCommandQueue1;
  void *GetCommandQueue2;
  void *GetFactoryCommandQueue1;
  void *GetFactoryCommandQueue2;
  /*Moho::MeshInstance*/ void *(__thiscall *UpdateEntityData)(UserEntity *_this,
                                                              int);
  void(__thiscall *UpdateVisibility)(int _this);
  void *RequiresUIRefresh;
  bool(__thiscall *IsSelectable)(UserUnit *_this);
  void *IsBeingBuilt;
  void *NotifyFocusArmyUnitDamaged;
  /*boost::detail::sp_counted_impl_pd*/ void *(__thiscall *CreateMeshInstance)(
      int _this, char);
  /*Moho::MeshRenderer*/ void *(__thiscall *DestroyMeshInstance)(void *_this);
};

struct Unit_ {};
struct VTransform {
  Vector4f orient;
  Vector3f pos;
  /* data */
};

struct IUnitVTable {
  void(__thiscall *IsUnit1)(Moho::Unit_ *);
  Moho::Unit_ *(__thiscall *IsUnit2)(Moho::Unit_ *);
  void(__thiscall *IsUserUnit1)(Moho::Unit_ *);
  void(__thiscall *IsUserUnit2)(Moho::Unit_ *);
  int(__thiscall *GetEntityId)(Moho::Unit_ *);
  Vector3f *(__thiscall *GetPosition)(Moho::Unit_ *);
  VTransform *(__thiscall *GetTransform)(Moho::Unit_ *);
  RUnitBlueprint *(__thiscall *GetBlueprint)(void *);
  LuaObject *(__thiscall *GetLuaObject)(Moho::Unit_ *, LuaObject *);
  //  int (__thiscall *CalcTransportLoadFactor)(Moho::Unit_ *);
  //  bool (__thiscall *IsDead)(Moho::IUnit *);
  //  bool (__thiscall *DestroyQueued)(Moho::IUnit *);
  //  int (__thiscall *IsMobile)(Moho::Unit_ *);
  //  bool (__thiscall *IsBeingBuilt)(Moho::Unit_ *);
  //  int (__thiscall *IsNavigatorIdle)(Moho::Unit_ *);
  //  unsigned __int8 (__thiscall *IsUnitState)(Moho::Unit_ *,
  //  Moho::EUnitState); Moho::UnitAttributes *(__thiscall
  //  *GetAttributes1)(Moho::IUnit *); Moho::Intel *(__thiscall
  //  *GetAttributes2)(Moho::Unit_ *); Moho::StatItem *(__thiscall
  //  *GetStatDefaultStr)(Moho::Unit_ *, const char *, std::string *);
  //  Moho::StatItem *(__thiscall *GetStatDefaultNum)(Moho::Unit_ *, const char
  //  *, float *); Moho::StatItem *(__thiscall *GetStatDefaultInt)(Moho::Unit_
  //  *, const char *, int *); Moho::StatItem *(__thiscall *GetStat)(Moho::Unit_
  //  *, const char *); void (__thiscall *SetAutoMode)(Moho::Unit_ *, BOOL);
  //  void (__thiscall *SetAutoSurfaceMode)(Moho::Unit_ *);
  //  void (__thiscall *IsAutoMode)(Moho::Unit_ *);
  //  int (__thiscall *IsAutoSurfaceMode)(Moho::Unit_ *);
  //  void (__thiscall *SetCustomName)(Moho::Unit_ *, std::string);
  //  std::string *(__thiscall *GetCustomName)(Moho::Unit_ *, std::string *);
};
UserEntityVTable *GetVTable(UserEntity *unit) { return (*(UserEntityVTable **)unit); }
IUnitVTable *GetIUnitVTable(UserUnit *unit) {
  return *(IUnitVTable **)((char *)unit + 0x148);
}

} // namespace Moho
VALIDATE_SIZE(Moho::CWldSession, 0x508);
VALIDATE_SIZE(Moho::struct_session_res3, 0x84);


// extern Moho::CWldSession* cwldsession asm("0x010A6470");