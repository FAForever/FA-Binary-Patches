#pragma once

#include "LuaAPI.h"
#include "Wm3.h"
#include "crt.h"

using EntId = uint32_t;
using CmdId = uint32_t;


template<class T>
inline constexpr T min(T a, T b) {
	return a > b ? b : a;
}
template<class T>
inline constexpr T max(T a, T b) {
	return a > b ? a : b;
}
inline constexpr int ifloor(float v) {
    return (int) v;
}
inline constexpr int iceil(float v) {
    return -ifloor(-v);
}



// hoist all forward declarations
struct CAiBuilderImpl;
struct CAiFormationDBImpl;
struct CAiFormationInstance;
struct CAiPersonality;
struct CAiReconDBImpl;
struct CAiSteeringImpl;
struct CAiTransportImpl;
struct CAniActor;
struct CAniPose;
struct CArmyImpl;
struct CArmyStats;
struct CCommandDB;
struct CDebugCanvas;
struct CDecalBuffer;
struct CD3DBatchTexture;
struct CEconomy;
struct CEffectManagerImpl;
struct CFormationInstance;
struct CInfluenceMap;
struct CSimConVarInstanceBase;
struct CSimSoundManager;
struct CSndParams;
struct CTask;
struct CTaskStage;
struct CTextureScroller;
struct CUnitCommand;
struct CUnitCommandQueue;
struct CUnitMotion;
struct CWldSession;
struct CWldTerrainRes;
struct Entity;
struct EntityDB;
struct EntityOccupationGrid;
struct HSndEntityLoop;
struct IAiCommandDispatch;
struct IAiNavigator;
struct IUnit;
struct MeshInstance;
struct PathTables;
struct PathQueue;
struct RBlueprint;
struct RScmResource;
struct Sim;
struct SParticleBuffer;
struct STIMap;
struct Unit;
struct UserUnit;
struct UserUnitManager;
struct WeakObject;



enum EAiTargetType : __int32
{
	AITARGET_None = 0,
	AITARGET_Entity = 1,
	AITARGET_Ground = 2,
};

enum ECollisionShape : __int32
{
    COLSHAPE_None = 0,
    COLSHAPE_Box = 1,
    COLSHAPE_Sphere = 2,
};

// just Moho::EIntel - 1 it appears
enum EEntityAttribute
{
	ENTATTR_Vision = 0,
	ENTATTR_WaterVision = 1,
	ENTATTR_Radar = 2,
	ENTATTR_Sonar = 3,
	ENTATTR_Omni = 4,
	ENTATTR_RadarStealthField = 5,
	ENTATTR_SonarStealthField = 6,
	ENTATTR_CloakField = 7,
	ENTATTR_Jammer = 8,
	ENTATTR_Spoof = 9,
	ENTATTR_Cloak = 10,
	ENTATTR_RadarStealth = 11,
	ENTATTR_SonarStealth = 12,
};

enum EEntityType : __int32
{
	ENTITYTYPE_Unit = 0x100,
	ENTITYTYPE_Prop = 0x200,
	ENTITYTYPE_Projectile = 0x400,
	ENTITYTYPE_Entity = 0x800,
};

enum EFireState : __int32
{
	FIRESTATE_ReturnFire = 0,
	FIRESTATE_HoldFire = 1,
	FIRESTATE_HoldGround = 2,
	FIRESTATE_Mix = -1,
};

enum EFootprintFlags : __int8
{
    FPFLAG_None = 0,
    FPFLAG_IgnoreStructures = 1,
};

enum EIntel : __int32
{
	INTEL_None = 0,
	INTEL_Vision = 1,
	INTEL_WaterVision = 2,
	INTEL_Radar = 3,
	INTEL_Sonar = 4,
	INTEL_Omni = 5,
	INTEL_RadarStealthField = 6,
	INTEL_SonarStealthField = 7,
	INTEL_CloakField = 8,
	INTEL_Jammer = 9,
	INTEL_Spoof = 10,
	INTEL_Cloak = 11,
	INTEL_RadarStealth = 12,
	INTEL_SonarStealth = 13,
};

enum EIntelCounter : __int32
{
	INTELCOUNTER_RadarStealthField = 0x1,
	INTELCOUNTER_SonarStealthField = 0x2,
	INTELCOUNTER_CloakField = 0x8,
};

enum EJobType : __int32
{
	JOB_None = 0,
	JOB_Build = 1,
	JOB_Repair = 2,
	JOB_Reclaim = 3,
};

enum ELayer : __int32
{
	LAYER_None = 0x0,
	LAYER_Land = 0x1,
	LAYER_Seabed = 0x2,
	LAYER_Sub = 0x4,
	LAYER_Water = 0x8,
	LAYER_Air = 0x10,
	LAYER_Orbit = 0x20,
	LAYER_All = 0x7F,
};

enum EOccupancyCaps : unsigned __int8
{
    OC_LAND = 0x1,
    OC_SEABED = 0x2,
    OC_SUB = 0x4,
    OC_WATER = 0x8,
    OC_AIR = 0x10,
    OC_ORBIT = 0x20,
    OC_ANY = 0xFF,
};

enum EReconFlags : __int32
{
	RECON_None = 0x0,
	RECON_Radar = 0x1,
	RECON_Sonar = 0x2,
	RECON_RadarSonar = 0x3,
	RECON_Omni = 0x4,
	RECON_AnyPing = 0x7,
	RECON_LOSNow = 0x8,
	RECON_AnySense = 0xF,
	RECON_LOSEver = 0x10,
	RECON_Exposed = 0x1C,
	RECON_KnownFake = 0x20,
	RECON_MaybeDead = 0x40,
};

enum ERuleBPUnitBuildRestriction : __int32
{
	RULEUBR_None = 0,
	RULEUBR_Bridge = 1,
	RULEUBR_OnMassDeposit = 2,
	RULEUBR_OnHydrocarbonDeposit = 3,
};

enum ERuleBPUnitCommandCaps : __int32
{
	RULEUCC_Move = 0x1,
	RULEUCC_Stop = 0x2,
	RULEUCC_Attack = 0x4,
	RULEUCC_Guard = 0x8,
	RULEUCC_Patrol = 0x10,
	RULEUCC_RetaliateToggle = 0x20,
	RULEUCC_Repair = 0x40,
	RULEUCC_Capture = 0x80,
	RULEUCC_Transport = 0x100,
	RULEUCC_CallTransport = 0x200,
	RULEUCC_Nuke = 0x400,
	RULEUCC_Tactical = 0x800,
	RULEUCC_Teleport = 0x1000,
	RULEUCC_Ferry = 0x2000,
	RULEUCC_SiloBuildTactical = 0x4000,
	RULEUCC_SiloBuildNuke = 0x8000,
	RULEUCC_Sacrifice = 0x10000,
	RULEUCC_Pause = 0x20000,
	RULEUCC_Overcharge = 0x40000,
	RULEUCC_Dive = 0x80000,
	RULEUCC_Reclaim = 0x100000,
	RULEUCC_SpecialAction = 0x200000,
	RULEUCC_Dock = 0x400000,
	RULEUCC_Script = 0x800000,
	RULEUCC_Invalid = 0x1000000,
};

enum ERuleBPUnitMovementType : __int32
{
	RULEUMT_None = 0,
	RULEUMT_Land = 1,
	RULEUMT_Air = 2,
	RULEUMT_Water = 3,
	RULEUMT_Biped = 4,
	RULEUMT_SurfacingSub = 5,
	RULEUMT_Amphibious = 6,
	RULEUMT_Hover = 7,
	RULEUMT_AmphibiousFloating = 8,
	RULEUMT_Spec = 9,
};

enum ERuleBPUnitToggleCaps : __int32
{
	RULEUTC_ShieldToggle = 0x1,
	RULEUTC_WeaponToggle = 0x2,
	RULEUTC_JammingToggle = 0x4,
	RULEUTC_IntelToggle = 0x8,
	RULEUTC_ProductionToggle = 0x10,
	RULEUTC_StealthToggle = 0x20,
	RULEUTC_GenericToggle = 0x40,
	RULEUTC_SpecialToggle = 0x80,
	RULEUTC_CloakToggle = 0x100,
};

enum ERuleBPUnitWeaponBallisticArc : __int32
{
	RULEUBA_None = 0,
	RULEUBA_LowArc = 1,
	RULEUBA_HighArc = 2,
};

enum ERuleBPUnitWeaponTargetType : __int32
{
	RULEWTT_Unit = 0,
	RULEWTT_Projectile = 1,
	RULEWTT_Prop = 2,
};

enum ESiloType : __int32
{
	SILOTYPE_Tactical = 0,
	SILOTYPE_Nuke = 1,
};

enum ETransportClass : __int32
{
	TRANSPORTCLASS_1 = 1,
	TRANSPORTCLASS_2 = 2,
	TRANSPORTCLASS_3 = 3,
	TRANSPORTCLASS_4 = 4,
	TRANSPORTCLASS_SPECIAL = 5,
};

enum EUnitState : __int32
{
	UNITSTATE_Immobile = 1,
	UNITSTATE_Moving = 2,
	UNITSTATE_Attacking = 3,
	UNITSTATE_Guarding = 4,
	UNITSTATE_Building = 5,
	UNITSTATE_Upgrading = 6,
	UNITSTATE_WaitingForTransport = 7,
	UNITSTATE_TransportLoading = 8,
	UNITSTATE_TransportUnloading = 9,
	UNITSTATE_MovingDown = 10,
	UNITSTATE_MovingUp = 11,
	UNITSTATE_Patrolling = 12,
	UNITSTATE_Busy = 13,
	UNITSTATE_Attached = 14,
	UNITSTATE_BeingReclaimed = 15,
	UNITSTATE_Repairing = 16,
	UNITSTATE_Diving = 17,
	UNITSTATE_Surfacing = 18,
	UNITSTATE_Teleporting = 19,
	UNITSTATE_Ferrying = 20,
	UNITSTATE_WaitForFerry = 21,
	UNITSTATE_AssistMoving = 22,
	UNITSTATE_PathFinding = 23,
	UNITSTATE_ProblemGettingToGoal = 24,
	UNITSTATE_NeedToTerminateTask = 25,
	UNITSTATE_Capturing = 26,
	UNITSTATE_BeingCaptured = 27,
	UNITSTATE_Reclaiming = 28,
	UNITSTATE_AssistingCommander = 29,
	UNITSTATE_Refueling = 30,
	UNITSTATE_GuardBusy = 31,
	UNITSTATE_ForceSpeedThrough = 32,
	UNITSTATE_UnSelectable = 33,
	UNITSTATE_DoNotTarget = 34,
	UNITSTATE_LandingOnPlatform = 35,
	UNITSTATE_CannotFindPlaceToLand = 36,
	UNITSTATE_BeingUpgraded = 37,
	UNITSTATE_Enhancing = 38,
	UNITSTATE_BeingBuilt = 39,
	UNITSTATE_NoReclaim = 40,
	UNITSTATE_NoCost = 41,
	UNITSTATE_BlockCommandQueue = 42,
	UNITSTATE_MakingAttackRun = 43,
	UNITSTATE_HoldingPattern = 44,
	UNITSTATE_SiloBuildingAmmo = 45,
};

enum EUnitStateMask : __int64
{
	UNITSTATEMASK_Immobile = 0x2LL,
	UNITSTATEMASK_Moving = 0x4LL,
	UNITSTATEMASK_Attacking = 0x8LL,
	UNITSTATEMASK_Guarding = 0x10LL,
	UNITSTATEMASK_Building = 0x20LL,
	UNITSTATEMASK_Upgrading = 0x40LL,
	UNITSTATEMASK_WaitingForTransport = 0x80LL,
	UNITSTATEMASK_TransportLoading = 0x100LL,
	UNITSTATEMASK_TransportUnloading = 0x200LL,
	UNITSTATEMASK_MovingDown = 0x400LL,
	UNITSTATEMASK_MovingUp = 0x800LL,
	UNITSTATEMASK_Patrolling = 0x1000LL,
	UNITSTATEMASK_Busy = 0x2000LL,
	UNITSTATEMASK_Attached = 0x4000LL,
	UNITSTATEMASK_BeingReclaimed = 0x8000LL,
	UNITSTATEMASK_Repairing = 0x10000LL,
	UNITSTATEMASK_Diving = 0x20000LL,
	UNITSTATEMASK_Surfacing = 0x40000LL,
	UNITSTATEMASK_Teleporting = 0x80000LL,
	UNITSTATEMASK_Ferrying = 0x100000LL,
	UNITSTATEMASK_WaitForFerry = 0x200000LL,
	UNITSTATEMASK_AssistMoving = 0x400000LL,
	UNITSTATEMASK_PathFinding = 0x800000LL,
	UNITSTATEMASK_ProblemGettingToGoal = 0x1000000LL,
	UNITSTATEMASK_NeedToTerminateTask = 0x2000000LL,
	UNITSTATEMASK_Capturing = 0x4000000LL,
	UNITSTATEMASK_BeingCaptured = 0x8000000LL,
	UNITSTATEMASK_Reclaiming = 0x10000000LL,
	UNITSTATEMASK_AssistingCommander = 0x20000000LL,
	UNITSTATEMASK_Refueling = 0x40000000LL,
	UNITSTATEMASK_GuardBusy = 0x80000000LL,
	UNITSTATEMASK_ForceSpeedThrough = 0x100000000LL,
	UNITSTATEMASK_UnSelectable = 0x200000000LL,
	UNITSTATEMASK_DoNotTarget = 0x400000000LL,
	UNITSTATEMASK_LandingOnPlatform = 0x800000000LL,
	UNITSTATEMASK_CannotFindPlaceToLand = 0x1000000000LL,
	UNITSTATEMASK_BeingUpgraded = 0x2000000000LL,
	UNITSTATEMASK_Enhancing = 0x4000000000LL,
	UNITSTATEMASK_BeingBuilt = 0x8000000000LL,
	UNITSTATEMASK_NoReclaim = 0x10000000000LL,
	UNITSTATEMASK_NoCost = 0x20000000000LL,
	UNITSTATEMASK_BlockCommandQueue = 0x40000000000LL,
	UNITSTATEMASK_MakingAttackRun = 0x80000000000LL,
	UNITSTATEMASK_HoldingPattern = 0x100000000000LL,
	UNITSTATEMASK_SiloBuildingAmmo = 0x200000000000LL,
};

enum EVisibilityMode : __int32
{
	VIZMODE_Never = 0x1,
	VIZMODE_Always = 0x2,
	VIZMODE_Intel = 0x4,
};

enum UnitWeaponRangeCategory : __int32
{
	UWRC_Undefined = 0,
	UWRC_DirectFire = 1,
	UWRC_IndirectFire = 2,
	UWRC_AntiAir = 3,
	UWRC_AntiNavy = 4,
	UWRC_Countermeasure = 5,
};



struct luaFuncDescReg
{						   // 0x1C bytes
	void **RegisterFunc;   // call for register lua function. 0x00E45E90 is std func
	const char *FuncName;  // lua name function
	const char *ClassName; // lua class name. 0x00E00D90 - <global> if none
	const char *FuncDesc;  // for log
	luaFuncDescReg *Next;  // reg func of chain
	lua_CFunction FuncPtr; // code address
	void *ClassPtr;		   // C++ class type address. NULL if class none
};
VALIDATE_SIZE(luaFuncDescReg, 0x1C);

template<class T>
struct fastvector
{
    T *mStart;
    T *mEnd;
    T *mCapacity;

    fastvector() {
        this->mStart = &this->mCapacity;
        this->mEnd = &this->mCapacity;
        this->mCapacity = &this->mCapacity;
    }
    ~fastvector() {
        delete[](this->mStart);
    }

    T *begin() { return this->mStart; }
    T *end() { return this->mEnd; }

    bool Empty() const {
        return this->mStart == this->mEnd;
    }
    size_t Size() const {
        return (this->mEnd - this->mStart) / sizeof(T);
    }
    size_t Capacity() const {
        return (this->mCapacity - this->mStart) / sizeof(T);
    }

    T &operator[](size_t idx) {
        return this->mStart[idx];
    }
};

template<class T, int N>
struct fastvector_n : public fastvector<T>
{
    // also stores the original capacity at [0] when not in use
    // (i.e. the zeroeth index in the inline vec)
    T *mOriginalVec;
    T mInlineVec[N];

    fastvector_n() {
        this->mStart = &this->mInlineVec[0];
        this->mEnd = &this->mInlineVec[0];
        this->mCapacity = &this->mInlineVec[N];
        this->mOriginalVec = &this->mInlineVec;
    }
    ~fastvector_n() {
        if (this->mStart != this->mOriginalVec) {
            delete[](this->mStart);
            this->mStart = this->mOriginalVec;
        }
    }
};

template<class T>
struct shared_ptr
{
	T *px;
	void *pn; // boost::detail::shared_count

	T &operator*() {
		return *this->px;
	}
	T *operator->() {
		return this->px;
	}
	T *get() {
		return this->px;
	}
	operator bool() {
		return this->px != nullptr;
	}
};

// T extends WeakObject
template<class T>
struct WeakPtr
{
	WeakObject *mObj;
	WeakPtr<T> *mNext;

	operator T*() {
		return this->get();
	}
	T &operator*() {
		return *this->get();
	}
	T *operator->() {
		return this->get();
	}
	T *get() {
		// this couldn't have been how it was done; since Moho::Unit inherits
		// Moho::WeakObject from both Moho::IUnit and Moho::Entity, it becomes
		// ambiguous and I've had to change all `WeakPtr<Unit>` to
		// `WeakPtr<IUnit>`
		return static_cast<T *>(this->mObj);
	}
};

struct WeakObject
{
	WeakPtr<void> *mNextUse;
};

template<class T>
struct TDatListItem
{
	TDatListItem<T> *mPrev;
	TDatListItem<T> *mNext;
};

template<class T>
struct TDatList : TDatListItem<T>
{
	TDatListItem<T> *begin() { return this->mPrev; }
	TDatListItem<T> *end() { return this->mPrev; }
};

struct BVIntSet
{
	uint32_t mStart;
	uint32_t gap;
    fastvector_n<uint32_t, 2> mBuckets;

    uint32_t BucketFor(uint32_t val) const {
        return (val >> 5) - this->mStart;
    }
	uint32_t MaskFor(uint32_t val) const {
		return 1 << (val & 0x1F);
	} 

	void EnsureBounds(uint32_t lower, uint32_t upper) {
		asm(
			"push %[lower];"
			"push %[self];"
			"call 0x00401980;"
			:
			: [self] "g" (this), [lower] "g" (lower), "c" (upper)
			:
		);
	}

	void Add(uint32_t val) {
		this->EnsureBounds(val, val + 1);
		uint32_t bucket = this->BucketFor(val);
		this->mBuckets[bucket] |= this->MaskFor(val);
	}

	void Clear(uint32_t val) {
		uint32_t bucket = this->BucketFor(val);
		this->mBuckets[bucket] &= ~this->MaskFor(val);
	}

	void Set(uint32_t val, bool add) {
		if (add) {
			this->Add(val);
		} else {
			this->Clear(val);
		}
	}
};
VALIDATE_SIZE(BVIntSet, 0x20);

template<class T, class U>
struct BVSet
{
    U mHelper;
	BVIntSet mSet;
};

struct RRuleGameRules
{
	void *vtable;
	CWldTerrainRes *res;
};

struct CDiskWatchListener
{
	void *vtable;
	TDatList<CDiskWatchListener> v4;
	void *mWatch; // CDiskWatch *
	vector<unk_t> mEvents; // vector<SDiskWatchEvent>
	vector<string> mPatterns;
};

struct EntityCategoryHelper
{
	RRuleGameRules *mRules;
	uint32_t gap;
};

using EntityCategory = BVSet<RBlueprint *, EntityCategoryHelper>;

struct SPointVector
{
  	Vector3f mPoint;
  	Vector3f mVector;
};

struct VMatrix4
{
    Vector4f d[4];
};

struct VTransform
{
	Quaternionf orient;
	Vector3f pos;
};

template<class T> 
struct SMinMax
{
	T mMin, mMax;
};

struct RObject
{ // 0x4 bytes
	void *vtable;
};

template <int T, int TInfo>
struct ObjectType
{
	const static int Type = T;
	const static int Info = TInfo;
};

struct CScriptObject : RObject, WeakObject
{
	int gap;
	LuaObject mCObject;
	LuaObject mLuaObject;
};
VALIDATE_SIZE(CScriptObject, 0x34);

struct mutex
{
	void *m_mutex;
	bool m_critical_section;
};

struct Stream
{ // 0x1C bytes
	void *vtable;
};

struct PipeStream : Stream
{ // 0x48 bytes
};

struct FileStream : Stream
{ // 0x00956DB4, 0x34 bytes
};

struct StatItem // : class TDatTreeItem<class Moho::StatItem>
{				// 0xA0 bytes
	void *vtable;
	// at 0x24
	int value; // int/float
	// at 0x74
	string name;
	// at 0x90
	int valueType;
	int unk2;
	// at 0x98
	void *criticalSection; // Result RtlInitializeCriticalSection
	bool unk1;			   // Set 1 at 00AC1A69, at 00AC1AB0 check(0 -> WaitForSingleObject, 1 -> RtlEnterCriticalSection)
};

struct EngineStats // : class Stats<class Moho::StatItem>
{				   // 0x50 bytes
	void *vtable;
	StatItem *stat;
	void *criticalSection; // Result RtlInitializeCriticalSection
	BOOL unk1;			   // Set 1 at 00AC1A69
	string str1;		   // Written "stats.log"
	string str2;
	int unk2;
	BOOL unk3;
};

struct Camera // : RCamCamera
{			  // 0x007A7972, 0x858 bytes
};

struct CMauiControl : CScriptObject
{ // 0x004C6F8A, 0x11C bytes
	using Type = ObjectType<0x10C7700, 0xF83314>;
};

struct CUIWorldView : CMauiControl
{ // 0x004C6F8A, 0x2A8 bytes
	using Type = ObjectType<0x10C77E4, 0xF8A71C>;
	// at 0x120
	Camera *camera;
	// at 0x208
	CWldSession *session;
	void *unk1; // If shift pressed

	bool GetCustomRenderingEnabled() const
	{
		return *(char *)((int)this + 377);
	}

	void SetCustomRenderingEnabled(bool state)
	{
		*(char *)((int)this + 377) = state;
	}
};

struct LaunchInfoNew
{ // 0x005423CC, 0xA4 bytes
	void *vtable;
	RRuleGameRules *rules;
	void *STIMap;
	// at 0xC
	string lua_gameMods;
	// at 0x28
	string str2;
	// at 0x60
	int unk1;
	int unk2;
	int unk3; // = -1
	// at 0x6C
	string interfaceLang;
	bool cheatsEnabled; // copied to CWldSession + 0x4D4
	char pad1[3];
	int unk4;
};

struct CTaskThread : TDatListItem<CTaskThread>, WeakObject
{
	CTaskStage *mStage;
	CTask *mTask;
	int val1;
	bool mStaged;
};

struct CTaskStage
{
  	TDatList<CTaskThread> mThreads;
	CTaskThread *mPrev;
	CTaskThread *mNext;
	bool unk;
};

struct REffectBlueprint : RObject
{
	uint32_t gap;
	string mBlueprintId;
	bool mHighFidelity;
	bool mMedFidelity;
	bool mLowFidelity;
};

struct RBeamBlueprint : REffectBlueprint
{ // 0x0050EEFD, 0x84 bytes
	float mLength;
	float mLifetime;
	float mThickness;
	float mUShift;
	float mVShift;
	string mTextureName;
	Vector4f mStartColor;
	Vector4f mEndColor;
	float mLODCutoff;
	float mRepeatRate;
	int mBlendmode;
};

struct RBlueprint : RObject
{ // ~0x60 bytes
	// at 0x4
	RRuleGameRules *owner;
	string name;
	string desc;
	// at 0x40
	string source; // example: /units/uel0001/uel0001_unit.bp
	uint32_t BlueprintOrdinal;
};
VALIDATE_SIZE(RBlueprint, 0x60);

struct RMeshBlueprintLOD
{
	string mMeshName;
	string mAlbedoName;
	string mSpecularName;
	string mLookupName;
	string mSecondaryName;
	string mShaderName;
	string mStr;
	float mLODCutoff;
	bool mScrolling;
	bool mOcclude;
	bool mSilhouette;
};

struct RMeshBlueprint : RBlueprint
{ // 0x0050DD83, 0x80 bytes
	vector<RMeshBlueprintLOD> mLODs;
	float mIconFadeInZoom;
	float mSortOrder;
	float mUniformScale;
	bool mStraddleWater;
};
VALIDATE_SIZE(RMeshBlueprint, 0x80);
struct SFootprint
{
    uint8_t mSizeX;
    uint8_t mSizeZ;
    EOccupancyCaps mOccupancyCaps;
    EFootprintFlags mFlags;
    float mMaxSlope;
    float mMinWaterDepth;
    float mMaxWaterDepth;
};

struct SNamedFootprint : SFootprint
{
	string mName;
	int mIndex;
};

struct REntityBlueprint : RBlueprint
{
    vector<string> mCategories;
    string mScriptModule;
    string mScriptClass;
    ECollisionShape mCollisionShape;
    float mSizeX;
    float mSizeY;
    float mSizeZ;
    float mAverageDensity;
    float mInertiaTensorX;
    float mInertiaTensorY;
    float mInertiaTensorZ;
    float mCollisionOffsetX;
    float mCollisionOffsetY;
    float mCollisionOffsetZ;
    int mDesiredShooterCap;
    SFootprint mFootprint;
    SFootprint mAltFootprint;
    bool mLifeBarRender;
    float mLifeBarOffset;
    float mLifeBarSize;
    float mLifeBarHeight;
    float mSelectionSizeX;
    float mSelectionSizeY;
    float mSelectionSizeZ;
    float mSelectionCenterOffsetX;
    float mSelectionCenterOffsetY;
    float mSelectionCenterOffsetZ;
    float mSelectionYOffset;
    float mSelectionMeshScaleX;
    float mSelectionMeshScaleY;
    float mSelectionMeshScaleZ;
    float mSelectionMeshUseTopAmount;
    float mSelectionThickness;
    float mUseOOBTestZoom;
    string mStrategicIconName;
    char mStrategicIconSortPriority;
    WeakPtr<unk_t> mStrategicIconRest;
    WeakPtr<unk_t> mStrategicIconSelected;
    WeakPtr<unk_t> mStrategicIconOver;
    WeakPtr<unk_t> mStrategicIconSelectedOver;
};

struct RPropBlueprint : REntityBlueprint
{ // 0x0050DD83, 0x1AC bytes
	
    struct RPropBlueprintDisplay
	{
		string mMeshBlueprint;
		float mUniformScale;
	} mDisplay;
    struct RPropBlueprintDefense
	{
		float mMaxHealth;
		float mHealth;
	} mDefense;
    struct RPropBlueprintEconomy
	{
		float mReclaimMassMax;
		float mReclaimEnergyMax;
	} mEconomy;
};
VALIDATE_SIZE(RPropBlueprint, 0x1AC);

struct RProjectileBlueprint : REntityBlueprint
{ // 0x0050DD83, 0x268 bytes
	
	string mDevStatus;
	struct RProjectileBlueprintDisplay
	{
		string mMeshBlueprint;
		float mUniformScale;
		float mMeshScaleRange;
		float mMeshScaleVelocity;
		float mMeshScaleVelocityRange;
		bool mCameraFollowsProjectile;
		float mCameraFollowTimeout;
		float mStrategicIconSize;
	} mDisplay;
	struct RProjectileBlueprintEconomy
	{
		float mBuildCostEnergy;
		float mBuildCostMass;
		float mBuildTime;
	} mEconomy;
	struct RProjectileBlueprintPhysics
	{
		bool mCollideSurface;
		bool mCollisionEntity;
		bool mTrackTarget;
		bool mVelocityAlign;
		bool mStayUpright;
		bool mLeadTarget;
		bool mStayUnderwater;
		bool mUseGravity;
		float mDetonateAboveHeight;
		float mDetonateBelowHeight;
		float mTurnRate;
		float mTurnRateRange;
		float mLifetime;
		float mLifeTimeRange;
		float mInitialSpeed;
		float mInitialSpeedRange;
		float mMaxSpeed;
		float mMaxSpeedRange;
		float mAcceleration;
		float mAccelerationRange;
		float mPositionX;
		float mPositionY;
		float mPositionZ;
		float mPositionXRange;
		float mPositionYRange;
		float mPositionZRange;
		float mDirectionX;
		float mDirectionY;
		float mDirectionZ;
		float mDirectionXRange;
		float mDirectionYRange;
		float mDirectionZRange;
		float mRotationalVelocity;
		float mRotationalVelocityRange;
		float mMaxZigZag;
		float mZigZagFrequency;
		bool mDestroyOnWater;
		int mMinBounceCount;
		int mMaxBounceCount;
		float mBounceVelDamp;
		bool mRealisticOrdinance;
		bool mStraightDownOrdinance;
	} mPhysics;
};
VALIDATE_SIZE(RProjectileBlueprint, 0x268);

struct RUnitBlueprintWeapon
{ // 0x184 bytes
	uint32_t gap[2];
	string mLabel;
	string mDisplayName;
	UnitWeaponRangeCategory mRangeCategory;
	bool mDummyWeapon;
	bool mPrefersPrimaryWeaponTarget;
	bool mStopOnPrimaryWeaponBusy;
	bool mSlavedToBody;
	float mSlavedToBodyArcRange;
	bool mAutoInitiateAttackCommand;
	float mTargetCheckInterval;
	bool mAlwaysRecheckTarget;
	float mMinRadius;
	float mMaxRadius;
	float mMaximumBeamLength;
	float mEffectiveRadius;
	float mMaxHeightDiff;
	float mTrackingRadius;
	float mHeadingArcCenter;
	float mHeadingArcRange;
	float mFiringTolerance;
	float mFiringRandomness;
	float mRequiresEnergy;
	float mRequiresMass;
	float mMuzzleVelocity;
	float mMuzzleVelocityRandom;
	float mMuzzleVelocityReduceDistance;
	bool mLeadTarget;
	float mProjectileLifetime;
	float mProjectileLifetimeUsesMultiplier;
	float mDamage;
	float mDamageRadius;
	string mDamageType;
	float mRateOfFire;
	string mProjectileId;
	ERuleBPUnitWeaponBallisticArc mBallisticArc;
	string mTargetRestrictOnlyAllow;
	string mTargetRestrictOnlyDisallow;
	bool mManualFire;
	bool mNukeWeapon;
	bool mOverChargeWeapon;
	bool mNeedPrep;
	bool mCountedProjectile;
	int mMaxProjectileStorage;
	bool mIgnoresAlly;
	ERuleBPUnitWeaponTargetType mTargetType;
	int mAttackGroundTries;
	bool mAimsStraightOnDisable;
	bool mTurreted;
	bool mYawOnlyOnTarget;
	bool mAboveWaterFireOnly;
	bool mBelowWaterFireOnly;
	bool mAboveWaterTargetsOnly;
	bool mBelowWaterTargetsonly;
	bool mReTargetOnMiss;
	bool mNeedToComputeBombDrop;
	float mBombDropThreshold;
	bool mUseFiringSolutionInsteadOfAimBone;
	bool mIgnoreIfDisabled;
	bool mCannotAttackGround;
	string mUIMinRangeVisualId;
	string mUIMaxRangeVisualId;
};
VALIDATE_SIZE(RUnitBlueprintWeapon, 0x184);

struct RUnitBlueprint : REntityBlueprint
{ // 0x0050DD83, 0x568 bytes
	// at 0x17C
	struct RUnitBlueprintGeneral
	{
		ERuleBPUnitCommandCaps mCommandCaps;
		ERuleBPUnitToggleCaps mToggleCaps;
		string mUpgradesTo;
		string mUpgradesFrom;
		string mUpgradesFromBase;
		string mSeedUnit;
		int mQuickSelectPriority;
		float mCapCost;
		int mSelectionPriority;
	} mGeneral;

	// at 0x200
	struct RUnitBlueprintDisplay
	{
		string mDisplayName;
		string mMeshBlueprint;
		string mPlaceholderMeshName;
		string mIconName;
		float mUniformScale;
		bool mSpawnRandomRotation;
		bool mHideLifebars;
	} mDisplay;

	// at 0x278
	struct RUnitBlueprintPhysics
	{
		bool mFlattenSkirt;
		float mSkirtOffsetX;
		float mSkirtOffsetZ;
		float mSkirtSizeX;
		float mSkirtSizeZ;
		float mMaxGroundVariation;
		ERuleBPUnitMovementType mMotionType;
		ERuleBPUnitMovementType mAltMotionType;
		bool mStandUpright;
		bool mSinkLower;
		bool mRotateBodyWhileMoving;
		float mDiveSurfaceSpeed;
		float mMaxSpeed;
		float mMaxSpeedReverse;
		float mMaxAcceleration;
		float mMaxBrake;
		float mMaxSteerForce;
		float mBankingSlope;
		float mRollStability;
		float mRollDamping;
		float mWobbleFactor;
		float mWobbleSpeed;
		float mTurnRadius;
		float mTurnRate;
		float mTurnFacingRate;
		bool mRotateOnSpot;
		float mRotateOnSpotThreshold;
		float mElevation;
		float mAttackElevation;
		float mCatchUpAcc;
		float mBackUpDistance;
		float mLayerChangeOffsetHeight;
		float mLayerTransitionDuration;
		ELayer mBuildOnLayerCaps;
		ERuleBPUnitBuildRestriction mBuildRestriction;
		SFootprint *mFootprint;
		SFootprint *mAltFootprint;
		float mFuelUseTime;
		float mFuelRechargeRate;
		float mGroundCollisionOffset;
		vector<float> mOccupRects;
		vector<float> mRaisedPlatforms;
	} mPhysics;

	// at 0x330
	struct RUnitBlueprintIntel
	{
		unsigned int mVisionRadius;
		unsigned int mWaterVisionRadius;
		unsigned int mRadarRadius;
		unsigned int mSonarRadius;
		unsigned int mOmniRadius;
		bool mRadarStealth;
		bool mSonarStealth;
		bool mCloak;
		bool mShowIntelOnSelect;
		unsigned int mRadarStealthFieldRadius;
		unsigned int mSonarStealthFieldRadius;
		unsigned int mCloakFieldRadius;
		SMinMax<float> mJamRadius;
		uint8_t mJammerBlips;
		SMinMax<float> mSpoofRadius;
	} mIntel;

	// at 0x368
	struct RUnitBlueprintAir
	{
		bool mCanFly;
		bool mWinged;
		bool mFlyInWater;
		float mAutoLandTime;
		float mMaxAirSpeed;
		float mMinAirSpeed;
		float mTurnSpeed;
		float mCombatTurnSpeed;
		float mStartTurnDistance;
		float mTightTurnMultiplier;
		float mSustainedTurnThreshold;
		float mLiftFactor;
		float mBankFactor;
		bool mBankForward;
		float mEngageDistance;
		float mBreakOffTriggger;
		float mBreakOffDistance;
		bool mBreakOffIfNearNewTarget;
		float mKMove;
		float mKMoveDamping;
		float mKLift;
		float mKLiftDamping;
		float mKTurn;
		float mKTurnDamping;
		float mKRoll;
		float mKRollDamping;
		float mCirclingTurnMult;
		float mCirclingRadiusChangeMinRatio;
		float mCirclingRadiusChangeMaxRatio;
		float mCirclingRadiusVsAirMult;
		float mCirclingElevationChangeRatio;
		float mCirclingFlightChangeFrequency;
		bool mCirclingDirChange;
		bool mHoverOverAttack;
		float mRandomBreakOffDistanceMult;
		float mRandomMinChangeCombatStateTime;
		float mRandomMaxChangeCombatStateTime;
		float mTransportHoverHeight;
		float mPredictAheadForBombDrop;
	} mAir;

	// at 0x3F8
	struct RUnitBlueprintTransport
	{
		ETransportClass mTransportClass;
		int mClassGenericUpTo;
		int mClass2AttachSize;
		int mClass3AttachSize;
		int mClass4AttachSize;
		int mClassSAttachSize;
		bool mAirClass;
		int mStorageSlots;
		int mDockingSlots;
		float mRepairsRate;
	} mTransport;

	// at 0x420
	struct RUnitBlueprintDefense
	{
		float mMaxHealth;
		float mHealth;
		float mRegenRate;
		float mAirThreatLevel;
		float mSurfaceThreatLevel;
		float mSubThreatLevel;
		float mEconomyThreatLevel;
		string mArmorType;
		struct RUnitBlueprintDefenseShield
		{
			float mShieldSize;
			float mRegenAssistMult;
		} mShield;
	} mDefense;

	// at 0x460
	struct RUnitBlueprintAI
	{
		float mGuardScanRadius;
		float mGuardReturnRadius;
		float mStagingPlatformScanRadius;
		bool mShowAssistRangeOnSelect;
		string mGuardFormationName;
		bool mNeedUnpack;
		bool mInitialAutoMode;
		string mBeaconName;
		vector<string> mTargetBones;
		float mRefuelingMultiplier;
		float mRefuelingRepairAmount;
		float mRepairConsumeEnergy;
		float mRepairConsumeMass;
		bool mAutoSurfaceToAttack;
		float mAttackAngle;
	} mAI;

	// at 0x4D8
	vector<RUnitBlueprintWeapon> mWeapon;
	uint32_t gap;
	// at 0x4E8
	struct RUnitBlueprintEconomy
	{
		float mBuildCostEnergy;
		float mBuildCostMass;
		float mBuildRate;
		float mBuildTime;
		float mStorageEnergy;
		float mStorageMass;
		bool mNaturalProducer;
		vector<string> mBuildableCategories;
		vector<string> mRebuildBonusIds;
		uint32_t gap;
		EntityCategory mCat;
		float mInitialRallyX;
		float mInitialRallyZ;
		bool mNeedToFaceTargetToBuild;
		float mSacrificeMassMult;
		float mSacrificeEnergyMult;
		float mMaxBuildDistance;
	} mEconomy;
};
VALIDATE_SIZE(RUnitBlueprint, 0x568);

struct SRuleFootprintsBlueprint
{
	list<SNamedFootprint> mFootprints;
	uint32_t gap;
};

struct REmitterCurveKey
{
	float mX;
	float mY;
	float mZ;
};

struct REmitterBlueprintCurve
{
	void *vtable;
	float mXRange;
	vector<REmitterCurveKey> mKeys;
};

struct REmitterBlueprint
{
	uint32_t gap1;
	string mBlueprintId;
	uint32_t gap2;
	REmitterBlueprintCurve mSizeCurve;
	REmitterBlueprintCurve mXDirectionCurve;
	REmitterBlueprintCurve mYDirectionCurve;
	REmitterBlueprintCurve mZDirectionCurve;
	REmitterBlueprintCurve mEmitRateCurve;
	REmitterBlueprintCurve mLifetimeCurve;
	REmitterBlueprintCurve mVelocityCurve;
	REmitterBlueprintCurve mXAccelCurve;
	REmitterBlueprintCurve mYAccelCurve;
	REmitterBlueprintCurve mZAccelCurve;
	REmitterBlueprintCurve mResistanceCurve;
	REmitterBlueprintCurve mStartSizeCurve;
	REmitterBlueprintCurve mEndSizeCurve;
	REmitterBlueprintCurve mInitialRotationCurve;
	REmitterBlueprintCurve mRotationRateCurve;
	REmitterBlueprintCurve mFrameRateCurve;
	REmitterBlueprintCurve mTextureSelectionCurve;
	REmitterBlueprintCurve mXPosCurve;
	REmitterBlueprintCurve mYPosCurve;
	REmitterBlueprintCurve mZPosCurve;
	REmitterBlueprintCurve mRampSelectionCurve;
	bool mLocalVelocity;
	bool mLocalAcceleration;
	bool mGravity;
	bool mAlignRotation;
	bool mAlignToBone;
	bool mEmitIfVisible;
	bool mCatchupEmit;
	bool mCreateIfVisible;
	bool mParticleResistance;
	bool mFlat;
	bool mInterpolateEmission;
	bool mSnapToWaterline;
	bool mOnlyEmitOnWater;
	float mTextureStripcount;
	float mSortOrder;
	float mLODCutoff;
	float mLifetime;
	float mRepeattime;
	float mTextureFramecount;
	int mBlendmode;
	string mTextureName;
	string mRampTextureName;
};

struct RTrailBlueprint
{
	uint32_t gap[2];
	string mBlueprintId;
	bool mHighFidelity;
	bool mMedFidelity;
	bool mLowFidelity;
	float mLifetime;
	float mTrailLength;
	float mSize;
	float mSortOrder;
	int mBlendMode;
	float mLODCutoff;
	bool mEmitIfVisible;
	bool mCatchupEmit;
	float mTextureRepeatRate;
	string mRepeatTexture;
	string mRampTexture;
};

struct RRuleGameRulesImpl : RRuleGameRules, CDiskWatchListener
{
	mutex mLock;
	LuaState *mState;
	fastvector<unk_t> mMaps;
	SRuleFootprintsBlueprint mFootprints;
	map<string, RUnitBlueprint*> mUnitBlueprints;
	map<string, RProjectileBlueprint*> mProjectileBlueprints;
	map<string, RPropBlueprint*> mPropBlueprints;
	map<string, RMeshBlueprint*> mMeshBlueprints;
	map<string, REmitterBlueprint*> mEmitterBlueprints;
	map<string, RBeamBlueprint*> mBeamBlueprints;
	map<string, RTrailBlueprint*> mTrailBlueprints;
	map<string, RBlueprint*> mBlueprints;
	vector<unk_t> mCategories;
};

struct CUIManager // : IUIManager
{				  // 0x0084C9CB, 0x78 bytes
	// at 0x30
	LuaState *LState; // from [10A6478]
};

struct Array2D
{
	char *mData;
	int mSizeX;
	int mSizeY;

	char &at(int x, int y) { return this->mData[x + y * this->mSizeX]; }
};

struct BitArray2D
{
	int *mData;
	int mSize;
	int mWidth;
	int mHeight;
};

struct SDelayedSubVizInfo
{
	Vector3f mPos;
	float mRadius;
	int mTick;
};

struct CIntelGrid
{
	STIMap *mMapData;
	Array2D mGrid;
	vector<SDelayedSubVizInfo> mUpdateList;
	int mGridSize;

	Circle2f ToGridCircle(Vector3f *pos, float radius);
	void Trace(Circle2f &prev, Circle2f &next);
};
VALIDATE_SIZE(CIntelGrid, 0x24);

struct SEconValue
{
	float ENERGY;
	float MASS;

    SEconValue(float e = 0, float m = 0) : ENERGY{e}, MASS{m}
    {}

    SEconValue operator+(const SEconValue &other)
    {
        return SEconValue{ENERGY + other.ENERGY, MASS + other.MASS};
    }

    SEconValue &operator+=(const SEconValue &other)
    {
        ENERGY += other.ENERGY;
        MASS += other.MASS;
        return *this;
    }
    SEconValue &operator-=(const SEconValue &other)
    {
        ENERGY -= other.ENERGY;
        MASS -= other.MASS;
        return *this;
    }

    SEconValue operator*(float mul)
    {
        return SEconValue{ENERGY * mul, MASS * mul};
    }

    SEconValue &operator=(const SEconValue &other)
    {
        ENERGY = other.ENERGY;
        MASS = other.MASS;
        return *this;
    }
};

struct SEconValueDouble
{
	double ENERGY;
	double MASS;
};

struct SEconTotals
{
	SEconValue mStored;
	SEconValue mIncome;
	SEconValue mReclaimed;
	SEconValue mLastUseRequested;
	SEconValue mLastUseActual;
	SEconValueDouble mMaxStorage;
};

struct SReconKey
{
  	WeakPtr<Entity> mEnt;
  	EntId mId;
};

struct SSTIUnitConstantData
{
	bool unk;
	shared_ptr<unk_t> mRoot; // boost::shared_ptr<Moho::Stats<Moho::StatItem>>
	bool mFake; // is jamming blip
};

struct UnitWeaponInfo
{
	EntityCategory mCat1;
	EntityCategory mCat2;
	ELayer mLayer;
	float mVal1;
	float mVal2;
	float mVal3;
	string mStr1;
	string mStr2;
};

struct UnitAttributes
{
	RUnitBlueprint *mBlueprint;
	uint32_t gap1;
	EntityCategory mRestrictionCategory;
	float mElevation;
	float mSpeedMult;
	float mAccMult;
	float mTurnMult;
	float mBreakOffTriggerMult;
	float mBreakOffDistMult;
	float mConsumptionPerSecondEnergy;
	float mConsumptionPerSecondMass;
	float mProductionPerSecondEnergy;
	float mProductionPerSecondMass;
	float mBuildRate;
	float mRegenRate;
	ERuleBPUnitCommandCaps mCommandCaps;
	ERuleBPUnitToggleCaps mToggleCaps;
	bool mReclaimable;
	bool mCapturable;
	unk_t unk;
};

struct SSTIUnitVariableData
{
	EntId mTarget;
	int mCreationTick;
	bool mAutoMode;
	bool mAutoSurfaceMode;
	bool mIsBusy;
	float mFuelRatio;
	float mShieldRatio;
	int mStunTicks;
	bool mIsPaused;
	bool mIsValidTarget;
	bool mRepeatQueue;
	EJobType mJobType;
	EFireState mFireState;
	float mWorkProgress;
	int mTacticalSiloBuildCount;
	int mNukeSiloBuildCount;
	int mTacticalSiloStorageCount;
	int mNukeSiloStorageCount;
	int mTacticalSiloMaxStorageCount;
	int mNukeSiloMaxStorageCount;
	EntId unk1;
	string mCustomName;
	SEconValue mProduced;
	SEconValue mResourcesSpent;
	SEconValue mMaintainenceCost;
	EntId mFocusUnit;
	EntId mGuardedUnit;
	EntId mTargetBlip;
	shared_ptr<CAniPose> mPriorSharedPose;
	shared_ptr<CAniPose> mSharedPose;
	int unk4;
	fastvector_n<CmdId, 8> mCommands;
	fastvector_n<CmdId, 8> mBuildQueue;
	fastvector_n<UnitWeaponInfo, 1> mWeaponInfo;
	UnitAttributes mAttributes;
	unsigned int mScriptbits;
	uint32_t gap2;
	EUnitStateMask mUnitStates;
	bool mDidRefresh;
	bool mOverchargePaused;
};

struct SPerArmyReconInfo
{
	bool mNeedsFlush;
	EReconFlags mReconFlags;
	void *mSTIMesh;
	shared_ptr<RScmResource> mMesh;
	shared_ptr<RScmResource> mPriorPose;
	shared_ptr<RScmResource> mPose;
	float mHealth;
	float mMaxHealth;
	float mFractionComplete;
  	bool mMaybeDead;
};
VALIDATE_SIZE(SPerArmyReconInfo, 0x34);

struct CIntelPosHandle
{
	struct vtable_t
	{
		void (__thiscall *AddViz)(CIntelPosHandle *self);
		void (__thiscall *SubViz)(CIntelPosHandle *self);
		void (__thiscall *dtr)(CIntelPosHandle *self, bool del);
	} *vtable;
	static vtable_t vftable asm("0x00E361C4");

	Vector3f mLastPos; // originally the last position checked; now the last position rastered
	float mRadius; // now a float
	int mLastTickUpdated; // no longer used
	bool mEnabled;
	shared_ptr<CIntelGrid> mGrid; // not used for counter intel, as they affect all other armies

	void __thiscall UpdatePos(Vector3f *newPos, int curTick);
	void __thiscall Update(Vector3f *newPos);
	/*virtual*/ void __thiscall AddViz();
	/*virtual*/ void __thiscall SubViz();
	/*pseudo-virtual*/ void TraceViz(Vector3f *newPos);
};

struct CIntelCounterHandle : CIntelPosHandle
{
	using CIntelPosHandle::vtable_t;
	static vtable_t vftable asm("0x00E361D4");

	Sim *mSim;
	CAiReconDBImpl *mReconDB;
	EIntelCounter mType;
	
	/*virtual*/ void __thiscall AddViz();
	/*virtual*/ void __thiscall SubViz();
	/*pseudo-virtual*/ void TraceViz(Vector3f *newPos);
};

struct CIntel
{
	CIntelPosHandle *mExplorationHandle; // unused
	CIntelPosHandle *mVisionHandle;
	CIntelPosHandle *mWaterHandle;
	CIntelPosHandle *mRadarHandle;
	CIntelPosHandle *mSonarHandle;
	CIntelPosHandle *mOmniHandle;
	CIntelCounterHandle *mRCIHandle;
	CIntelCounterHandle *mSCIHandle;
	CIntelCounterHandle *mVCIHandle;
	bool mJamming;
	bool mJammingEnabled;
	bool mSpoof;
	bool mSpoofEnabled;
	bool mCloak;
	bool mCloakEnabled;
	bool mRadarStealth;
	bool mRadarStealthEnabled;
	bool mSonarStealth;
	bool mSonarStealthEnabled;

	void Update(Vector3f *newPos, int curTick);
};

class EntityAttributes
{
	unsigned int mVisionRange;
	unsigned int mWaterVisionRange;
	unsigned int mRadarRange;
	unsigned int mSonarRange;
	unsigned int mOmniRange;
	unsigned int mRadarStealthRange;
	unsigned int mSonarStealthRange;
	unsigned int mCloakRange;

public:
	unsigned int GetIntelRadius(EEntityAttribute attribute) const {
		switch (attribute) {
		case ENTATTR_Vision:
			return mVisionRange & 0x7FFFFFFF;
		case ENTATTR_WaterVision:
			return mWaterVisionRange & 0x7FFFFFFF;
		case ENTATTR_Radar:
			return mRadarRange & 0x7FFFFFFF;
		case ENTATTR_Sonar:
			return mSonarRange & 0x7FFFFFFF;
		case ENTATTR_Omni:
			return mOmniRange & 0x7FFFFFFF;

		case ENTATTR_Cloak:
			return mCloakRange & 0x7FFFFFFF;
		case ENTATTR_RadarStealth:
			return mRadarStealthRange & 0x7FFFFFFF;
		case ENTATTR_SonarStealth:
			return mSonarStealthRange & 0x7FFFFFFF;
		case ENTATTR_RadarStealthField:
		case ENTATTR_SonarStealthField:
		case ENTATTR_CloakField:
		case ENTATTR_Jammer:
		case ENTATTR_Spoof:
		default:
			return 0;
		}
	}
	void SetIntelRadius(EEntityAttribute attribute, unsigned int radius) {
		switch (attribute) {
		case ENTATTR_Vision:
			mVisionRange = radius | (mVisionRange & 0x80000000);
			break;
		case ENTATTR_WaterVision:
			mWaterVisionRange = radius | (mWaterVisionRange & 0x80000000);
			break;
		case ENTATTR_Radar:
			mRadarRange = radius | (mRadarRange & 0x80000000);
			break;
		case ENTATTR_Sonar:
			mSonarRange = radius | (mSonarRange & 0x80000000);
			break;
		case ENTATTR_Omni:
			mOmniRange = radius | (mOmniRange & 0x80000000);
			break;
		case ENTATTR_Cloak:
			mCloakRange = radius | (mCloakRange & 0x80000000);
			break;
		case ENTATTR_RadarStealth:
			mRadarStealthRange = radius | (mRadarStealthRange & 0x80000000);
			break;
		case ENTATTR_SonarStealth:
			mSonarStealthRange = radius | (mSonarStealthRange & 0x80000000);
			break;
		default:
			break;
		}
	}
};

struct IClientManager
{
	void *vtable;
};

struct IClient
{
	void *vtable;
};

struct CWldMap
{
	void *zero1;
	void *zero2;
	void *zero3;
};

struct SSTICommandSource
{ // 0x24 bytes
	int index;
	string name;
	int protocol; // -1 SinglePlayer, 3 MultiPlayer
};

struct ResourceDeposit
{						// 0x14 bytes
	int X1, Z1, X2, Z2; // Rect
	int Type;			// 1 - Mass, 2 - Energy
};
VALIDATE_SIZE(ResourceDeposit, 0x14);

struct CSimResources // : ISimResources // : IResources
{					 // 0x007444EF, 0x1C bytes
	void *vtable;
	// at 0x8 in vtable
	// ecx:CreateResourceDeposit(type, x, y, z, size)
	// at 0x28 in vtable
	// ecx:FindResourceDeposit(PtrPosXZ, PtrResultXZ, Radius, Type):Bool

	uint8_t pad[0xC];
	// at 0x10
	fastvector<ResourceDeposit> deposits;
};
VALIDATE_SIZE(CSimResources, 0x1C);

struct SWldSessionInfo
{ // 0x30 bytes
	string map_name;

	// at 0x1C
	void *LaunchInfoNew;

	// at 0x20
	int unk1;
	bool isBeingRecorded;
	bool isReplay;
	bool isMultiplayer;
	char pad1;
	// at 0x2C
	int ourCmdSource;

	IClientManager *clientManager;
	int unk2; // = 255 possibly cmdSourceIndex
};

struct SSTIArmyVariableData
{
	SEconTotals mEcon;
	bool mResourceSharing;
	uint32_t gap1;
	BVIntSet mNeutrals;
	BVIntSet mAllies;
	BVIntSet mEnemies;
	bool mIsAlly;
	uint32_t gap2;
	BVIntSet mValidCommandSources;
	uint32_t mPlayerColor;
	uint32_t mArmyColor;
	string mArmyType;
	int mFaction;
	bool mUseWholeMap;
	vector<unk_t> mVec;
	uint32_t gap3;
	bool mShowScore;
	uint32_t gap4;
	EntityCategory mCat;
	bool mIsOutOfGame;
	Vector2f mArmystart;
	int mNoRushTimer;
	float mNoRushRadius;
	float mNoRushOffSetX;
	float mNoRushOffSetY;
	float mHasHandicap;
	float mHandicap;
	uint32_t gap5;
};
VALIDATE_SIZE(SSTIArmyVariableData, 0x160);

struct SSTIArmyConstantData
{
	int mIndex;
	string mArmyName;
	string mPlayerName;
	bool mIsCivilian;
	shared_ptr<CIntelGrid> mVisionReconGrid;
	shared_ptr<CIntelGrid> mWaterReconGrid;
	shared_ptr<CIntelGrid> mRadarReconGrid;
	shared_ptr<CIntelGrid> mSonarReconGrid;
	shared_ptr<CIntelGrid> mOmniReconGrid;
	shared_ptr<CIntelGrid> mRCIReconGrid;
	shared_ptr<CIntelGrid> mSCIReconGrid;
	shared_ptr<CIntelGrid> mVCIReconGrid;
};
VALIDATE_SIZE(SSTIArmyConstantData, 0x80);

struct IArmy
{
	SSTIArmyConstantData mConstDat;
	SSTIArmyVariableData mVarDat;
	
	bool IsAlly(int armyIndex) const {
		if ( armyIndex == -1 )
			return 0;
		auto v3 = (armyIndex >> 5) - mVarDat.mAllies.mStart;
		return v3 < mVarDat.mAllies.mBuckets.mEnd - mVarDat.mAllies.mBuckets.mStart
			&& ((mVarDat.mAllies.mBuckets.mStart[v3] >> (armyIndex & 31)) & 1) != 0;
	}
};
VALIDATE_SIZE(IArmy, 0x1E0);

struct BaseArmy
{
	void *vtable;
	unk_t unk;
};

struct SimArmy : BaseArmy, IArmy
{
};
VALIDATE_SIZE(SimArmy, 0x1E8);

struct UserArmy : IArmy
{ // 0x210 bytes
	uint32_t gap1;
	// at 0x1E4
	CWldSession *mSession;
	uint32_t gap2[4];
  	map<unk_t, unk_t> mEngineers;
  	map<unk_t, unk_t> mFactories;
	
};
VALIDATE_SIZE(UserArmy, 0x210);

struct SBuildReserveInfo
{
  	WeakPtr<IUnit> mUnit;
  	WeakPtr<CUnitCommand> mCom;
};

struct CAiBrain : CScriptObject
{
	CArmyImpl *mArmy;
	CArmyImpl *mCurrentEnemy;
	CAiPersonality *mPersonality;
	string mCurrentPlan;
	vector<SPointVector> mAttackVectors;
	uint32_t gap;
	EntityCategory mCat;
	map<Vector2i, SBuildReserveInfo> mBuildStructurePositions;
	Sim *mSim2;
	CTaskStage *mAiThreads;
	CTaskStage *mAttackerThreads;
	CTaskStage *mThreads;
};

struct CEconStorage
{
	CEconomy *mEconomy;
	SEconValue mAmt;
};

struct CEconRequest : TDatListItem<CEconRequest>
{
	SEconValue mPerSecond;
	SEconValue mAddWhenSetOff;
};

struct CEconomy
{
	Sim *mSim;
	int mIndex;
	SEconValue mResources;
	SEconValue v4;
	SEconTotals mTotals;
	CEconStorage *extraStorage;
	bool mResourceSharing;
	TDatList<CEconRequest> mConsumptionData;
};


struct CAiTarget
{
  	EAiTargetType targetType;
	TDatList<CAiTarget> mList;
	Vector3f mPosition;
	int mTargetPoint;
	bool mTargetIsMobile;
};

struct ITask
{
	void *vtable;
	unk_t unk;
	bool *mDestroyed;
	CTaskThread *mTaskThread;
	CTask *mSubtask;
};

struct CTask : ITask
{
  	bool mIsOwning;
};
VALIDATE_SIZE(CTask, 0x18);

struct SSTIEntityAttachInfo
{
	EntId mEnt;
};

struct SEntAttachInfo
{
	int *v0;
	int v1;
	int mBone;
	int v3;
	VTransform mParentOrientation;
};

struct SSTIEntityVariableData
{
	shared_ptr<RScmResource> mScmResource;
	RMeshBlueprint *mMesh;
	Vector3f mScale;
	float mHealth;
	float mMaxHealth;
	bool mIsBeingBuilt;
	bool mIsDead;
	bool mRequestRefreshUI;
	VTransform mCurTransform;
	VTransform mLastTransform;
	float mCurImpactSomething;
	float mFractionComplete;
	EntId mAttachmentParent;
	fastvector_n<SSTIEntityAttachInfo, 1> mAttachInfo;
	uint32_t unk;
	Vector2f mScroll1;
	Vector2f mScroll2;
	CSndParams *mAmbientSound;
	CSndParams *mRumbleSound;
	bool mNotVisibility;
	EVisibilityMode mVisibility;
	ELayer mLayer;
	bool mUsingAltFp;
	shared_ptr<CD3DBatchTexture> mUnderlayTexture;
	EntityAttributes mAttributes;
};
VALIDATE_OFFSET(SSTIEntityVariableData, mAttributes, 0xB0);

struct SSTIEntityConstantData
{
  	EntId mId;
  	REntityBlueprint *mBlueprint;
  	uint32_t mTickCreated;
};

struct PositionHistory
{
  	VTransform mHistory[25];
  	int mCurrent;
};

struct CColPrimitive
{
    void *vtable;
};

struct SPhysBody
{
	float mGravity;
	float mMass;
	Vector3f mInvInertiaTensor;
	Vector3f mCollisionOffset;
	Vector3f mPos;
	Quaternionf mOrientation;
	Vector3f mVelocity;
	Vector3f mWorldImpulse;
};

struct Motor
{
	void *vtable;
};

struct MotorFallDown : Motor, CScriptObject
{
	float mDirection;
	float mAngle;
	float mDepth;
  	bool mFlag;
};

struct MotorSinkAway : Motor, CScriptObject
{
  	float mDY;
};

template<class T>
struct EntitySetTemplate : TDatListItem<EntitySetTemplate<T>>
{
  	fastvector_n<T *, 4> mVec;
};

struct CollisionDBRect
{
	unsigned __int16 xPos;
	unsigned __int16 xSize;
	unsigned __int16 zPos;
	unsigned __int16 zSize;
};

struct CollisionShapeBase : CollisionDBRect
{
	EntityOccupationGrid *mGrid;
	bool mMarked;
};

struct Entity : CScriptObject, CTask, CollisionShapeBase
{
	EEntityType mType;
	TDatList<CAiTarget> mTargets;
	SSTIEntityConstantData mConstDat;
	int32_t gap1;
	SSTIEntityVariableData mVarDat;
	Sim *mSim;
	CArmyImpl *mArmy;
	VTransform mLastTrans;
	PositionHistory *mPositionHistory;
	float mLastImpactSomething;
	int mLastTickProcessed;
	CColPrimitive *mCollisionShape;
	vector<Entity *> mAttachedEntities;
	SEntAttachInfo mAttachInfo;
	bool unk1;
	bool mDestroyQueued;
	bool mDestroyed;
	string mResId;
	CIntel *mIntelManager;
	EVisibilityMode mVizToFocusPlayer;
	EVisibilityMode mVizToAllies;
	EVisibilityMode mVizToEnemies;
	EVisibilityMode mVizToNeutrals;
	bool mInterfaceCreated;
	CTextureScroller *mScroller;
	SPhysBody *mPhysBody;
	bool unk2;
	string mUniqueName;
	EntitySetTemplate<Entity> mShooters;
	AxisAlignedBox3f mCollision;
	LuaObject mObjPos;
	Motor *mMotor;
};
VALIDATE_OFFSET(Entity, mType, 0x5C);
VALIDATE_OFFSET(Entity, mSim, 0x148);
VALIDATE_SIZE(Entity, 0x270);

struct Shield : Entity
{};

struct ReconBlip : Entity
{
	WeakPtr<IUnit> mCreator;
	bool mDeleteWhenStale;
	Vector3f mJamOffset;
	SSTIUnitConstantData mConstDat;
	SSTIUnitVariableData mVarDat;
	vector<SPerArmyReconInfo> mReconDat;
};
VALIDATE_OFFSET(ReconBlip, mReconDat, 0x4C0);
VALIDATE_SIZE(ReconBlip, 0x4D0);

struct CPlatoon : CScriptObject
{
	using Type = ObjectType<0x10C6FCC, 0xF6A1FC>;
};

struct CArmyImpl : SimArmy
{
	Sim *mSim;
	CAiBrain *mBrain;
	CAiReconDBImpl *mReconDB;
	CEconomy *mEconomy;
	string mArmyPlans;
	CArmyStats *mArmyStats;
	CInfluenceMap *mIMmap;
	PathQueue *mPathFinder;
	uint32_t gap1[2];
	fastvector_n<CPlatoon *, 8> mPlatoons;
	vector<EntitySetTemplate<Unit>> v149;
	int v153;
	int v154;
	float mUnitCap;
	bool mIgnoreUnitCap;
	uint32_t mPathCapLand;
	uint32_t mPathCapSea;
	uint32_t mPathCapBoth;
	uint32_t unk;
};
VALIDATE_SIZE(CArmyImpl, 0x288);

struct CAiReconDBImpl // : IAiReconDB
{					  // 0x005BFFB8, 0xB0 bytes
	void *vtable;
	map<SReconKey, ReconBlip> mBlipMap;
	vector<ReconBlip> mBlipList;
 	vector<ReconBlip> mOrphans;
	CArmyImpl *mArmy;
	STIMap *mMapData;
	Sim *mSim;
	CInfluenceMap *mIMap;
	shared_ptr<CIntelGrid> mVisionGrid;
	shared_ptr<CIntelGrid> mWaterGrid;
	shared_ptr<CIntelGrid> mRadarGrid;
	shared_ptr<CIntelGrid> mSonarGrid;
	shared_ptr<CIntelGrid> mOmniGrid;
	shared_ptr<CIntelGrid> mRCIGrid;
	shared_ptr<CIntelGrid> mSCIGrid;
	shared_ptr<CIntelGrid> mVCIGrid;
	EntityCategory unk1;
	bool mFogOfWar;
	int unk2;
};

struct Entities
{ // 0x50 bytes
};

struct EntityChain // [[Entities+4]+4]
{
	void *next;
	// void *?;
	// void *?;
	int ID;
	void *entity;
};

struct Projectile : Entity
{ // 0x004C702B, 0x380 bytes
	// at 0x6C
	using Type = ObjectType<0x10C7578, 0xF77914>;
	RProjectileBlueprint *Blueprint;
};

struct Prop : Entity
{ // 0x004C702B, 0x288 bytes
	// at 0x6C
	RPropBlueprint *Blueprint;
};

struct CCommandTask
{ // 0x34 bytes
	void *vtable;
	void *unk0; // self+4;
	// at 0x10
	CCommandTask *prevTask; // ?
	// at 0x1C
	Unit *owner;
	Sim *sim;
	// at 0x28
	// CCommandTask *prevTask; -0x2C
};

struct CUnitMobileBuildTask : CCommandTask
{ // 0xE8 bytes
	// at 0xC
	Unit *owner;
	Sim *sim;
	Unit *target0;		// -4
	CCommandTask *next; // ?
	// at 0x2C
	string name;
	float FractionComplete;
	// at 0x50
	CUnitCommand *unitCommand;
	RUnitBlueprint *build;
	float unk0, unk1, unk2, unk3;
	// at 0x84
	Unit *target1; // -4
};

struct CUnitRepairTask : CCommandTask
{ // 0x9C bytes
	// at 0xC
	Unit *owner;
	Sim *sim;
	// at 0x2C
	string name;
	// at 0x50
	CUnitCommand *unitCommand;
	Unit *target; // -4
};

struct CUnitCommand : CScriptObject
{ // 0x178 bytes
	// at 0x34
	CCommandTask *task; // -4
	// at 0x40
	Sim *sim;
	// at 0x4C
	float unk3;
	// at 0x5C
	float unk4;
	// at 0x60
	RUnitBlueprint *BpBuild;
	string unk5;
	// at 0x98
	uint32_t CommandType;
	// at 0xA0
	uint32_t TargetId;
	Vector4f Pos;
	// at 0x120
	Entity Target; //-0x4
	// at 0x128
	Vector3f Pos2;
	// at 0x160
	LuaObject Nil;
	void *unk6;
};

struct SCommand
{						// 0x8 bytes
	void *CUnitCommand; // +0x4
	void *nil;
};

struct CommandQueue
{ // 0x28 bytes
	void *unk1;
	void *unk2;
	Unit *Owner;
	vector<SCommand> Commands;
	void *unk3;
	void *unk4;
	bool unk5;
};

struct CEconomyEvent
{

};

struct UnitWeapon //: CScriptEvent
{				  // 0x006D3114, 0x188 bytes
	void *vtable;
	// at 0x10
	void *vtable2;
	// at 0x1C
	LuaObject UserData;
	LuaObject Table;
	// at 0x5C
	float RateOfFire;
	float MinRadius;
	float MaxRadius;
	float SquaredMinRadius;
	float SquaredMaxRadius;
	// at 0xA0
	Unit *Owner;
};

struct CAiAttackerImpl // : IAiAttacker
{					   // 0x005D6ABF, 0xA4 bytes
	void *vtable;
	// at 0x58
	vector<UnitWeapon *> Weapons;
};

struct UserUnitWeapon
{ // 0x98 bytes
	// at 0x54
	float MinRadius;
	float MaxRadius;
};

struct SSiloBuildInfo
{
	UnitWeapon *mWeapon;
	int mAmmo;
	int gap;
};

struct CAiSiloBuildImpl
{
    void *vtable;
    Unit *mUnit;
	SSiloBuildInfo mSiloInfo[2];
	list<ESiloType> mSiloTypes;
	CEconRequest *mRequest;
	int mState; // Moho::ESiloBuildStage
	SEconValue mSegmentCost;
	SEconValue mSegmentSpent;
	float mSegments;
	int mCurSegments;
};
VALIDATE_SIZE(CAiSiloBuildImpl, 0x4C);

struct SInfoCache
{
  	CFormationInstance *mFormationLayer;
	WeakPtr<IUnit> v1;
	int v2;
	bool v3;
	float mTopSpeed;
	float v4;
	Vector3f v5;
};

struct IUnit_vtable
{
	struct vtable_t
	{
		Unit *(__thiscall *IsUnit1)(IUnit *);
		Unit *(__thiscall *IsUnit2)(IUnit *);
		UserUnit *(__thiscall *IsUserUnit1)(IUnit *);
		UserUnit *(__thiscall *IsUserUnit2)(IUnit *);
		EntId &(__thiscall *GetEntityId)(IUnit *, /*out*/ EntId &);
		Vector3f *(__thiscall *GetPosition)(IUnit *);
		VTransform *(__thiscall *GetTransform)(IUnit *);
		RUnitBlueprint *(__thiscall *GetBlueprint)(IUnit *);
		LuaObject &(__thiscall *GetLuaObject)(IUnit *, /*out*/ LuaObject &);
		double (__thiscall *CalcTransportLoadFactor)(Unit *);
		bool (__thiscall *IsDead)(IUnit *);
		bool (__thiscall *DestroyQueued)(IUnit *);
		bool (__thiscall *IsMobile)(IUnit *);
		bool (__thiscall *IsBeingBuilt)(IUnit *);
		int (__thiscall *IsNavigatorIdle)(IUnit *);
		bool (__thiscall *IsUnitState)(IUnit *, EUnitState);
		UnitAttributes *(__thiscall *GetAttributes1)(IUnit *);
		UnitAttributes *(__thiscall *GetAttributes2)(IUnit *);
		StatItem *(__thiscall *GetStatDefaultStr)(IUnit *, const char *, string &def);
		StatItem *(__thiscall *GetStatDefaultNum)(IUnit *, const char *, float &def);
		StatItem *(__thiscall *GetStatDefaultInt)(IUnit *, const char *, int &def);
		StatItem *(__thiscall *GetStat)(IUnit *, const char *);
	} *vtable;
};

struct IUnit : IUnit_vtable, WeakObject
{
};

struct Unit : IUnit, Entity
{
	using Type = ObjectType<0x010C6FC8, 0x00F6A1E4>;
	
	SSTIUnitConstantData mUnitConstDat;
	SSTIUnitVariableData mUnitVarDat;
	CUnitMotion *mUnitMotion;
	CUnitCommandQueue *mCommandQueue;
	WeakPtr<IUnit> mCreator;
	WeakPtr<IUnit> mTransportedBy;
	WeakPtr<IUnit> mFerryUnit;
	WeakPtr<Entity> mFocusEntity;
	WeakPtr<Entity> mTargetBlip;
	WeakPtr<IUnit> mGuardedUnit;
	Vector3f v96;
	uint32_t gap2;
	EntitySetTemplate<Unit> mGuards;
	CAiFormationInstance *mGuardFormation;
	bool mScheduledCleanUp;
	int mCreationTick;
	CEconStorage *mExtraStorage;
	bool v114;
	CEconRequest *mConsumptionData;
	bool mConsumptionIsActive;
	bool mProductionActive;
	bool v116c;
	bool v116d;
	float mResourceConsumed;
	CAniActor *mAniActor;
	CAiAttackerImpl *mAttacker;
	IAiCommandDispatch *mCommandDispatch;
	IAiNavigator *mNavigator;
	CAiSteeringImpl *mSteering;
	CAiBuilderImpl *mBuilder;
	CAiSiloBuildImpl *mSiloBuild;
	CAiTransportImpl *mTransport;
	bool mIsOccupying;
	float mTransportLoadFactor;
	map<string, float> mArmor;
	TDatList<CEconomyEvent> mEconEvents;
	uint8_t mTerrainType;
	bool mDebugAIStates;
	SInfoCache mInfo;
	Rect2i mOccupation;
	fastvector_n<WeakPtr<Entity>, 20> mBlipsInRange;
	uint32_t mBlipLastUpdateTick;
	uint32_t v193;
	fastvector_n<ReconBlip *, 2> mReconBlips;
	bool mIsNotPod;
	bool mIsEngineer;
	bool mIsNaval;
	bool mIsAir;
	bool mUsesGridBasedMotion;
	bool mIsMelee;
	bool mHasFocus;
	int v202;
	uint32_t gap3[5];
};
VALIDATE_OFFSET(Unit, mUnitMotion, 0x4B0);
VALIDATE_OFFSET(Unit, mArmor, 0x568);
VALIDATE_OFFSET(Unit, mReconBlips, 0x670);
VALIDATE_SIZE(Unit, 0x6A8);

struct DBEntry
{
	void *mDB; // Moho::SpatialDB<Moho::MeshInstance> *
	uint32_t mEntry;
};

struct VisionDB
{
  	unk_t mVisionDB;
  	struct Pool
	{
		void* vtable;
		list<unk_t> mList1;
		list<unk_t> mList2;
	} mPool;
  	struct Entry
	{
		Entry *mPrev1;
		Entry *mPrev2;
		Entry *mNext;
		bool unk;
		bool mVis;
		Circle2f mPrevCicle;
		Circle2f mCurCircle;
	} *mRoot;

	struct Handle
	{
  		void *vtable;
  		VisionDB *mDB;
  		Entry *mNode;
	};
};

struct SCreateEntityParams : SSTIEntityConstantData
{};

struct UserEntity
{
	void *vtable;
	uint32_t unk1[2];
	CWldSession *mSession;
	DBEntry mEntry;
	VisionDB::Handle *mVisionHandle;
	shared_ptr<CAniPose> mPose;
	shared_ptr<CAniPose> v9;
	MeshInstance *mMeshInstance;
	unk_t unk2[2];
	CSndParams *mAmbientSnd;
	HSndEntityLoop *mRumbleSnd;
	int mCreationTickMaybe;
	SCreateEntityParams mParams;
	SSTIEntityVariableData mVarDat;
	UserArmy *mArmy;
	VTransform mTransform;
	float mLastInterpAmt;
	bool unk3;
	bool unk4;
};
VALIDATE_OFFSET(UserEntity, mVarDat, 0x50);
VALIDATE_OFFSET(UserEntity, mArmy, 0x120);
VALIDATE_SIZE(UserEntity, 0x148);

struct UserUnit : UserEntity, IUnit, CScriptObject
{ // 0x008B8601, 0x3E8 bytes
	using Type = ObjectType<0x10C77AC, 0xF881E0>;

	SSTIUnitConstantData mUnitConstDat;
    bool unk1;
	SSTIUnitVariableData mUnitVarDat;
	unk_t mCreator;
	unk_t unk2;
	UserUnitManager *mManager;
	UserUnitManager *mFactoryManager;
	map<unk_t, unk_t> mSelectionSets;
	bool unk3;
	bool mIsEngineer;
	bool mIsFactory;
	bool unk4;
	unk_t unk5;
	unk_t unk6;

	inline ERuleBPUnitToggleCaps GetToggleCaps() {
		return this->IUnit::vtable->GetAttributes2(this)->mToggleCaps;
	}
};
VALIDATE_OFFSET(UserUnit, mUnitConstDat, 0x184);
VALIDATE_OFFSET(UserUnit, mUnitVarDat, 0x198);
VALIDATE_OFFSET(UserUnit, mManager, 0x3C8);
VALIDATE_SIZE(UserUnit, 0x3E8);

struct CMauiBitmap : public CMauiControl
{
	using Type = ObjectType<0x10C7704, 0xF832F4>;
};

struct CMersenneTwister
{
	unsigned int mState[624];
	int mPos;

    unsigned int IRand() {
		uint32_t i;
		asm(
			"call 0x0040E9F0"
			:
			: "d" (this), "a" (i)
			: "memory"
		);
		return i;
	}
};

static float int_to_float asm("0x00E4F7B0"); // 2.3283064e-10
struct CRandomStream
{
	CMersenneTwister mTwister;
	float mMarsagliaPair;
	bool mHasMarsagliaPair;

	float FRand() { // parity with 0x0040EA70
		return this->mTwister.IRand() * int_to_float;
	}
	float FRand(float min, float max) { // parity with 0x0051B5C0
		return min + (max - min) * FRand();
	}
	float FRandGaussian() {
		float val;
		asm(
			"call    0x0040EEC0;"
			: "=t" (val)
			: "S" (this)
			: "memory"
		);
		return val;
	}
};
VALIDATE_SIZE(CRandomStream, 0x9CC);

struct MD5Block
{
	char mVals[56];
	__int64 mSize;
};

struct MD5Digest
{
  	int mVals[4];
};

struct MD5Context
{
	MD5Digest mDigest;
	MD5Block mBlock;
	unsigned int mPos;
	int gap;
	__int64 mSize;
};

struct EntityOccupationGrid
{
	int mWidth;
	int mHeight;
	int mLastIndex;
	int mGridWidth;
	vector<CollisionShapeBase> **mUnits;
	vector<CollisionShapeBase> **mProps;
	vector<CollisionShapeBase> **mEntities;
	int unk1;
	int unk2;
	int unk3;
	void **start;
	void **end;
	int unk4;
};

struct COGrid
{
	Sim *mSim;
	EntityOccupationGrid mEntityGrid;
	BitArray2D mTerrainOccupation;
	BitArray2D mWaterOccupation;
	BitArray2D mOccupation;
};
VALIDATE_SIZE(COGrid, 0x68);

struct SDesyncInfo
{
	int mBeat;
	int mArmy;
	MD5Digest mHash1;
	MD5Digest mHash2;
};

struct CGeomSolid3
{
    fastvector_n<Plane3f, 6> mVec;
};

struct GeomCamera3
{
	VTransform mTranform;
	VMatrix4 mProjection;
	VMatrix4 mView;
	VMatrix4 mViewProjection;
	VMatrix4 mInverseProjection;
	VMatrix4 mInverseView;
	VMatrix4 mInverseViewProjection;
	uint32_t alignment;
	CGeomSolid3 mSolid1;
	CGeomSolid3 mSolid2;
	float mLOCScale;
	VMatrix4 mViewport;
	uint32_t unk;
};
VALIDATE_SIZE(GeomCamera3, 0x2C8);

struct SSyncFilter
{
	int mFocusArmy;
	vector<GeomCamera3> mCams;
	uint32_t mGap1[3];
	BVIntSet mSet1;
	bool mBool;
	uint32_t mGap2[3];
	BVIntSet mSet2;
};

struct SPhysConstants
{
    Vector3f mGravity;
};

struct SCamShakeParams
{
	float v1;
	float v2;
	float v3;
	float v4;
	float v5;
	float v6;
	float minTime;
};

struct RDebugOverlay : TDatListItem<RDebugOverlay>
{
	virtual ~RDebugOverlay(); // force vtable
};
VALIDATE_SIZE(RDebugOverlay, 0xC);

struct Sim // : ICommandSink
{
	void *vtable;
	string str0;
	FILE *mLog;
	string str1;
	list<unk_t> list1;
	bool mDesyncFreeMaybe;
	MD5Context mContext;
	MD5Digest mSimHashes[128];
	vector<SDesyncInfo> mDesyncs;
	CEffectManagerImpl *mEffectManager;
	CSimSoundManager *mSoundManager;
	RRuleGameRulesImpl *mRules;
	STIMap *mMapData;
	shared_ptr<CSimResources> mSimResources;
	LuaState *mLuaState;
	bool mGameEnded;
	bool mGameOver;
	int mPausedBy;
	bool mSingleStep;
	bool mNeedsToSyncMaybe;
	bool mCheatsEnabled;
	vector<int> mCheaters;
	int mCurBeat;
	bool mDidProcess;
	unsigned int mCurTick;
	CRandomStream *mRandom;
	COGrid *mOGrid;
	vector<CArmyImpl *> mArmiesList; // vector<SimArmy *>, but this is more useful
	vector<SSTICommandSource> mCommandSources;
	unsigned int mCurCommandSource;
	CTaskStage mThread3;
	CTaskStage mTaskThread;
	CTaskStage mThread1;
	shared_ptr<CDebugCanvas> mDebugCanvas1;
	shared_ptr<CDebugCanvas> mDebugCanvas2;
	PathTables *mPathTables;
	CAiFormationDBImpl *mFormationDB;
	EntityDB *mEntityDB;
	CCommandDB *mCommandDB;
	int mCommandCount;
	unk_t unk1;
	shared_ptr<SParticleBuffer> mParticleBuffer;
	CDecalBuffer *mDecalBuffer;
	TDatList<RDebugOverlay> mDebugOverlays;
	vector<CSimConVarInstanceBase *> mSimVars;
	vector<unk_t> mSyncVec1;
	vector<SCamShakeParams> mSyncCamShake;
	vector<unk_t> mSyncVec4;
	vector<unk_t> mSyncVec5;
	Rect2i mPlayableRect1;
	Rect2i mPlayableRect2;
	vector<string> mPrintField;
	vector<unk_t> mSyncVec3;
	SPhysConstants *mPhysConstants;
	list<Shield *> mShields;
	queue<Entity *> mDeletionQueue;
	TDatList<unk_t> mCoordEntities;
	bool mRequestXMLArmyStatsSubmit;
	int mSyncArmy;
	bool mDidSync;
	struct SyncSizes
	{
		int mAudioRequests;
		int mArmyData;
		int mEntityData;
		int mSize4;
		int mSize5;
	} mSyncSizes;
	void *unk2;
	SSyncFilter mSyncfilter;
};
VALIDATE_OFFSET(Sim, mRandom, 0x904);
VALIDATE_SIZE(Sim, 0xAF8);

struct CWldSession
{ // 0x0089318A, 0x508 bytes
	CWldSession *self1;
	CWldSession *self2;
	void *self_weird1; // this + 0x8
	void *self_weird2; // this + 0x8
	// at 0x10
	LuaState *LState; // Set from constructor argument
	void *unk1;		  // 0x14 bytes
	RRuleGameRules *rules;
	CWldMap *map;
	LaunchInfoNew *launchInfo;
	void *unk2; // class detail::boost::sp_counted_impl_p<struct Moho::LaunchInfoNew>
	string mapName;
	Entities entities;
	uint8_t pad1[0x3A8];
	// at 0x3F0
	list<UserArmy *> armies;
	// at 0x3FC
	void *unk3; // 0xCC0 bytes
	void *unk4; // 0x64 bytes
	uint8_t pad2[0x20];
	// at 0x424
	CSimResources *deposits;
	uint8_t pad3[0x48];
	// at 0x470
	vector<SSTICommandSource> cmdSources;
	// at 0x480
	int ourCmdSource;
	bool isReplay;
	bool isBeingRecorded;
	bool isMultiplayer;
	bool allowObservers;

	int focusArmyIndex; // focused army, -1 = observer

	BOOL isGameOver;
	uint8_t pad4[0x10];
	// at 0x4A0
	::map<UserUnit*, WeakPtr<UserEntity>> selectedUnits;
	size_t mMaxSelectionSize;
	// at 0x4B0
	struct
	{ // size 0x24
		void *unk1;
		float mouseWorldPosX;
		float mouseWorldPosY;
		float mouseWorldPosZ;
		void *unk2;
		void *unk3;
		int highlightCommandId;
		float mouseScreenPosX;
		float mouseScreenPosY;
	} mouse;
	// at 0x4D4
	bool cheatsEnabled; // Copied from LaunchInfoNew + 0x88
	uint8_t pad5[0x13];
	// at 0x4E8
	bool displayEconomyOverlay;
	bool relationsArmyColors;
	uint8_t pad6[0x1C];
};
VALIDATE_SIZE(CWldSession, 0x508);

struct CSimDriver // : ISTIDriver
{				  // 0x0073B59E, 0x230 bytes
	void *vtable;
	Sim *sim;
	IClientManager *ClientManager;
	uint8_t pad1[4];
	// at 0x10
	void *LaunchInfoNew; // From g_SWldSessionInfo, only when loading game init
	uint8_t pad2[8];
	// at 0x1C
	uint32_t beatCounter1;
	uint32_t beatCounter2; // Copied to address 0x1290710
	uint32_t beatCounter3;
	uint8_t pad3[0x86];
	// at 0xB0
	int focusArmyIndex;
	uint8_t pad4[0x6C];
	// at 0x120
	int focusArmyIndex2; // Copy from 0xB0
	uint8_t _pad5[0x104];
	// at 0x228
	int maxSimRate; // from CalcMaxSimRate
	uint8_t pad6[4];
};
VALIDATE_SIZE(CSimDriver, 0x230);

struct iGrid_data4
{
	SMinMax<__int16> *mData;
	int mWidth;
	int mHeight;
};

struct iGrid_data2
{
	__int16 *data;
	int mWidth;
	int mHeight;
};

struct iGrid
{
    iGrid_data4 data1;
    iGrid_data2 data2;
};

struct CHeightField : iGrid_data2
{
	vector<iGrid> mGrids;
};
VALIDATE_SIZE(CHeightField, 0x1C);

struct TerrainTypes
{
    fastvector_n<LuaObject, 256> mTypes;
};

struct STIMap
{
	shared_ptr<CHeightField> mHeightField;
	Rect2i mPlayableRect;
	TerrainTypes mTerrainTypes;
	Array2D mTerrainType;
	bool mBlocking[256];
	bool mWaterEnabled;
	float mWaterElevation;
	float mWaterElevationDeep;
	float mWaterElevationAbyss;
	int unk;
};
VALIDATE_SIZE(STIMap, 0x1548);

struct CPushTask
{
	char data[0x24];
};

typedef CPushTask CPullTask;

struct INetConnector
{
	void *vtable;
};

struct CLobby
{ // 0x004C702B, 0xC8 bytes
	// at 0x20
	LuaObject unk1;

	int unk2;
	int unk3;
	int unk4;
	int unk5;
	// at 0x44
	CPushTask pushTask;

	// at 0x60
	CPullTask pullTask;

	// at 0x84
	bool hosted_or_joined; // Has CLobby been used to host/join yet?

	// at 0x88
	void *not_host; // 0 if we're host

	// at 0x90
	string our_name;
	// at 0xAC
	int our_UID;

	// at 0xB0
	struct
	{
		INetConnector *first_element;
		INetConnector *last_element; // It will probably always be our CNetXXXConnector
	} peer_list;					 // Probably singly-linked list
};

struct CLobbyPeer
{ // 0x50 bytes

	CLobbyPeer *next; // Doubly linked list pointers
	CLobbyPeer *prev;

	string playerName;

	// at 0x24
	int ownerID; // User UID
	int const1;
	short const2;
	char pad1[2];

	// at 0x30
	int const3;	  // enum?
	float const4; // = 0
	int const5;
	set<int> mConnectedTo;
	int cmdSourceIndex; // == 255 => Unassigned
	int playerNo;

	/*
		const1, const2, const3, const5
		specially define the CLobbyUser

		const3:
		1 - Host for a lobby we joined?
		3 - Peer who connected to us?
		6 - Game Peer of a Launched Game?
	*/
};

struct CClientManagerImpl : IClientManager
{ // 0x184D0 bytes
	// at 0x40C
	mutex mLock;

	// at 0x420
	list<IClient *> mClients;
	INetConnector *mConnector;

	// at 0x434
	bool mWeAreReady;
	bool mEveryoneIsReady;
	char pad1[2];
	int mDispatchedBeat;
	int mAvailableBeat;
	int mFullyQueuedBeat;
	int mPartiallyQueuedBeat;
	int gameSpeedChangeCounter; // mGameSpeedClock
	int mGameSpeedRequester;	// Always 0
	int gameSpeedRequested;		// mGameSpeed
	bool speedControlEnabled;
	// at 0x458
	uint32_t hEvent; // for KERNEL32.SetEvent
	// at 0x18470
	PipeStream mPipe;
	// at 0x184BC
	bool unk1; // if value is 1 then KERNEL32.SetEvent is bypassed
};

struct EjectRequest
{
	const char *mRequester;
	int mAfterBeat;
};

struct CClientBase : IClient
{ // 0x0053B5E9, 0xD8 bytes
	string mNickname;
	// at 0x20
	int mIndex; // client index
	int mUID;
	IClientManager *clientManager;

	// at 0x30
	BVIntSet unk1;
	// at 0x50
	int mCommandSource;
	bool mReady;
	char pad1[3];
	PipeStream mPipe;

	// at 0xA0
	int mQueuedBeat;
	int mDispatchedBeat;
	int mAvailableBeatRemote;
	vector<int> mLatestAckReceived;
	int mLatestBeatDispatchedRemote;
	// at 0xC0
	bool mEjectPending;
	bool mEjected;
	char pad2[2];
	vector<EjectRequest> mEjectRequests;
	int maxSimRate; // from CalcMaxSimRate
};

struct CLocalClient : CClientBase
{ // 0x0053B5E9, 0xD8 bytes
}; // equal CClientBase

struct CReplayClient : CClientBase
{ // 0x160 bytes
	// before 0xD8 it CClientBase
	// at 0xD8
	FileStream *stream;
	// at 0xE0
	void *ptr1; // self+0xF0
	void *ptr2; // self+0xF0
	void *ptr3; // self+0x130
	void *ptr4; // self+0xF0
	void *unk1;
	// at 0x130
	void *unk2;
	// at 0x138
	uint32_t replayBeat;
	bool unk3;
	void *unk4;
	uint32_t hSemaphore1;
	uint32_t hSemaphore2;
	uint32_t hMutex;
	// at 0x15C
	bool unk5;
	bool unk6;
};

struct CNetUDPConnection
{
};

struct CNetUDPConnector // : INetConnector
{					   // 0x18090 bytes
	void *vtable;
	void *smth; // Listen socket fd?
	mutex mMutex;
	// at 0x14
	SOCKET mSocket;
	// at 0x24
	TDatList<CNetUDPConnection> mConnections;
};
/*Game Types
  Multiplayer - CLobby::LaunchGame
  Replay - VCR_SetupReplaySession
  SinglePlayer - WLD_SetupSessionInfo
  Saved Game - CSavedGame::CreateSinglePlayerSession

  Session State
	0 - None?
	1 - Loading?
	2 - Started?
	3 - SIM Initialized
	4 - SIM Started
	5 - Game Started
	7 - Restart Requested
	8 - Session Halted
*/

struct MapImager
{ // 0x14 bytes
	void *vtable;
};

struct MeshThumbnailRenderer
{ // 0x3C bytes
	void *vtable;
};

struct RangeRenderer
{ // 0x94 bytes
	void *vtable;
};

struct VisionRenderer
{ // 0x78 bytes
	void *vtable;
};

struct BoundaryRenderer
{ // 0x68 bytes
	void *vtable;
};

struct Shadow
{ // 0x318 bytes
	void *vtable;
};

struct Clutter
{ // 0x192C bytes
	void *vtable;
};

struct Silhouette
{ // 0x74? bytes
	void *vtable;
	// at 0xC
	CUIWorldView *worldView;
};

struct WRenViewport // : WD3DViewport
{					// 0x0097977D, 0x21A8 bytes
	void *vtable;
	// at 0x32C
	MapImager mapImager;
	// at 0x340
	MeshThumbnailRenderer meshThumbnailRenderer;
	// at 0x37C
	RangeRenderer rangeRenderer;
	// at 0x410
	VisionRenderer visionRenderer;
	// at 0x488
	BoundaryRenderer boundaryRenderer;
	// at 0x4F0
	Shadow shadow;
	// at 0x808
	Clutter clutter;
	// at 0x2134
	Silhouette silhouette;
};

namespace incomplete
{
	struct Command
	{
		Command *self;
		uint8_t pad1[0x1C];
		RBlueprint *bpBuild; // at 0x20
		uint8_t pad2[0x34];
		int commandType; // at 0x58 // see EUnitCommandType; sub_552170
		uint8_t pad3[0x4];
		int targetId; // at 0x60 // valid if != 0xF0000000
		Vector3f pos; // at 0x64
	};
}
