#pragma once

#include "LuaAPI.h"

typedef int BOOL;
typedef int unk_t;

struct luaFuncDescReg
{	// 0x1C bytes
	void **RegisterFunc;  // call for register lua function. 0x00E45E90 is std func
	char *FuncName;       // lua name function
	char *ClassName;      // lua class name. 0x00E00D90 - <global> if none
	char *FuncDesc;       // for log
	luaFuncDescReg *Next; // reg func of chain
	void *FuncPtr;        // code address
	void *ClassPtr;       // C++ class type address. NULL if class none
};
VALIDATE_SIZE(luaFuncDescReg, 0x1C)

// Probably from visual c++ 9
struct vtable;
struct typeInfo
{	// 0x8+ bytes
	void *vtable;
	int zero;
	char name[];
};

struct classDesc
{	// 0x30+ bytes
	// at 0x4
	uint32_t trueDataOffset; // Subtraction
	// at 0xC
	void *typeInfo;
	// at 0x20
	void *beginParents; // +0x4
	void *endParents; // -0x4
	// at 0x28
	classDesc *parents[];
	//void* typeInfo;
};

struct vtable
{	// 0x8+ bytes
	// at -0x4
	void* classDesc;
	void* methods[];
};


template<typename T>
struct vector
{	// 0x10 bytes
	uint32_t pad;
	T *begin, *end, *capacity_end;

	T operator[](int i) { return begin[i]; }
	size_t size() { return begin ? end - begin : 0; }
};
VALIDATE_SIZE(vector<unk_t>, 0x10)

template<typename T>
struct list
{       // 0xC bytes
	T *begin, *end, *capacity_end;

	T operator[](int i) { return begin[i]; }
	size_t size() { return begin ? end - begin : 0; }
};
VALIDATE_SIZE(list<unk_t>, 0xC)

template<typename T>
struct linked_list
{
};

struct moho_set
{       // 0x20 bytes
	int baseI;
	int unk1;
	uint32_t *begin, *end, *capacity_end;
	void *unk2;
	uint32_t value; // Memory for 'Short Set Optimization'
	void *unk3;

	void set(uint32_t item, bool set) {
		auto *itemPtr = &begin[item >> 5 - baseI];
		if (itemPtr >= end) end = itemPtr + 1;
		item = 1 << (item & 0x1F);
		if (set)
			*itemPtr |= item; else
			*itemPtr &= ~item;
	}
	bool operator[](int item) {
		auto *itemPtr = &begin[item >> 5 - baseI];
		if (itemPtr >= end) return false;
		return *itemPtr & (1 << (item & 0x1F));
	}
};
VALIDATE_SIZE(moho_set, 0x20)

typedef int SOCKET;
// GPGCore

struct Vector2f
{	// 0x8 bytes
	float x,z;
};

struct Vector3f
{	// 0xC bytes
	float x,y,z;
};

struct Vector4f
{	// 0x10 bytes
	float x,y,z,w;
};

struct RObject
{	// ~0x10 bytes
	void *vtable;
};

template <int T, int TInfo>
struct ObjectType {
	const static int Type = T;
	const static int Info = TInfo;
};

struct CScriptObject : RObject
{//0x004C6F8A, 0x3C bytes
};

struct WeakObject
{	// 0x8 bytes?
	void *vtable;
	void *Unk1;
};

struct gpg_mutex
{
	int unk1;
	bool unk2; // If value is 0 then NTDLL.RtlEnterCriticalSection is bypassed
};

struct Stream
{	// 0x1C bytes
	void *vtable;
};

struct PipeStream : Stream
{	// 0x48 bytes
};

struct FileStream : Stream
{//0x00956DB4, 0x34 bytes
};

struct StatItem // : class TDatTreeItem<class Moho::StatItem>
{	// 0xA0 bytes
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
	bool unk1; // Set 1 at 00AC1A69, at 00AC1AB0 check(0 -> WaitForSingleObject, 1 -> RtlEnterCriticalSection)
};

struct EngineStats // : class Stats<class Moho::StatItem>
{	// 0x50 bytes
	void *vtable;
	StatItem *stat;
	void *criticalSection; // Result RtlInitializeCriticalSection
	BOOL unk1; // Set 1 at 00AC1A69
	string str1; // Written "stats.log"
	string str2;
	int unk2;
	BOOL unk3;
};

struct Camera // : RCamCamera
{//0x007A7972, 0x858 bytes
};

struct CMauiControl : CScriptObject
{//0x004C6F8A, 0x11C bytes
	using Type = ObjectType<0x10C7700, 0xF83314>;
};

struct CWldSession;

struct CUIWorldView : CMauiControl
{//0x004C6F8A, 0x2A8 bytes
	using Type = ObjectType<0x10C77E4, 0xF8A71C>;
	// at 0x120
	Camera *camera;
	// at 0x208
	CWldSession *session;
	void *unk1; // If shift pressed
};

struct RBlueprint;

struct RRuleGameRules
{//0x00529158, 0xD0 bytes
	void *vtable;
	// at 0x2C
	//list L1;
	// at 0x48
	//list L2;
	// at 0xB8
	list<RBlueprint*> Blueprints;
	//list L3;
	// at 0xC4
	//void *Blueprints;
	//void *Start, *End;
};

struct LaunchInfoNew
{//0x005423CC, 0xA4 bytes
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

struct REffectBlueprint : RObject
{
};

struct RBeamBlueprint : REffectBlueprint
{//0x0050EEFD, 0x84 bytes
};

struct RBlueprint : RObject
{	// ~0x60 bytes
	// at 0x4
	RRuleGameRules *owner;
	string name;
	string desc;
	// at 0x40
	string source; // example: /units/uel0001/uel0001_unit.bp
	uint32_t BlueprintOrdinal;
};

struct RMeshBlueprint : RBlueprint
{//0x0050DD83, 0x80 bytes
	// at 0x70
	float IconFadeInZoom;
};

struct REntityBlueprint : RBlueprint
{	// ~0x17C bytes
	// at 0x60
	vector<string> Categories;

	// at 0xD8
	struct SFootprint {
		char SizeX;
		char SizeZ;
		char OccupancyCaps;
		char Flags;
		float MaxSlope;
		float MinWaterDepth;
		float unk1;
	} Footprint, AltFootprint;
};

struct RPropBlueprint : REntityBlueprint
{//0x0050DD83, 0x1AC bytes
};

struct RProjectileBlueprint : REntityBlueprint
{//0x0050DD83, 0x268 bytes
};

struct RUnitBlueprintWeapon
{	// 0x184 bytes
};

struct RUnitBlueprint : REntityBlueprint
{//0x0050DD83, 0x568 bytes
	// at 0x17C
	struct RUnitBlueprintGeneral {
	} General;

	// at 0x200
	struct RUnitBlueprintDisplay {
	} Display;

	// at 0x278
	struct RUnitBlueprintPhysics {
	} Physics;

	// at 0x330
	struct RUnitBlueprintIntel {
	} Intel;

	// at 0x368
	struct RUnitBlueprintAir {
	} Air;

	// at 0x3F8
	struct RUnitBlueprintTransport {
		// at 0x400
		int T2ClassSize;
		int T3ClassSize;
		int GenericClassSize;
		int SClassSize;
	} Transport;

	// at 0x420
	struct RUnitBlueprintDefense {
		// at 0x43C
		string ArmorType;
	} Defense;

	// at 0x460
	struct RUnitBlueprintAI {
		float GuardScanRadius;
		float GuardReturnRadius;
		float StagingPlatformScanRadius;
		bool ShowAssistRangeOnSelect;
		string GuardFormationName;
		bool NeedUnpack;
		bool InitialAutoMode;
		string BeaconName;
		vector<string> TargetBones;
		float RefuelingMultiplier;
		float RefuelingRepairAmount;
		float RepairConsumeEnergy;
		float RepairConsumeMass;
		bool AutoSurfaceToAttack;
		float AttackAngle;
	} AI;

	// at 0x4D8
	vector<RUnitBlueprintWeapon> Weapon;

	// at 0x4E8
	struct RUnitBlueprintEconomy {
		// at 0x564
		float MaxBuildDistance;
	} Economy;
};

struct CUIManager // : IUIManager
{//0x0084C9CB, 0x78 bytes
	// at 0x30
	LuaState *LState; // from [10A6478]
};

struct CAiReconDBImpl // : IAiReconDB
{//0x005BFFB8, 0xB0 bytes
	void *vtable;
	// at 0xA8
	bool FogOfWar;
};

struct CIntelGrid
{	// 0x24 bytes
};

struct IClientManager
{
	void *vtable;
};

struct IClient
{
	void* vtable;
};

struct CWldMap
{
	void *zero1;
	void *zero2;
	void *zero3;
};

struct SSTICommandSource
{       // 0x24 bytes
	int index;
	string name;
	int protocol; // -1 SinglePlayer, 3 MultiPlayer
};

struct Deposit
{	// 0x14 bytes
	int X1,Z1,X2,Z2; // Rect
	int Type; // 1 - Mass, 2 - Energy
};
VALIDATE_SIZE(Deposit, 0x14);

struct CSimResources // : ISimResources // : IResources
{//0x007444EF, 0x1C bytes
	void *vtable;
	// at 0x8 in vtable
	// ecx:CreateResourceDeposit(type, x, y, z, size)
	// at 0x28 in vtable
	// ecx:FindResourceDeposit(PtrPosXZ, PtrResultXZ, Radius, Type):Bool

	uint8_t pad[0xC];
	// at 0x10
	list<Deposit> deposits;
};
VALIDATE_SIZE(CSimResources, 0x1C)

struct SWldSessionInfo
{	// 0x30 bytes
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

struct SimArmyEconomyInfo
{	// 0x60 bytes
	void *unk1;
	int unk2;
	float _incomeEnergy;    // div 10
	float _incomeMass;      // div 10

	float baseIncomeEnergy; // div 10
	float baseIncomeMass;   // div 10
	float storedEnergy;
	float storedMass;

	float incomeEnergy;     // div 10
	float incomeMass;       // div 10
	float reclaimedEnergy;
	float reclaimedMass;

	float requestedEnergy;  // div 10
	float requestedMass;    // div 10
	float expenseEnergy;    // div 10
	float expenseMass;      // div 10

	uint32_t maxEnergy;
	int unk3;
	uint32_t maxMass;
	int unk4;

	float unk5;
	uint32_t isResourceSharing;
	float unk6;
	float unk7;
};

struct BaseArmy
{	// 0x1DC bytes
	// struct 0x80 bytes
	int armyIndex;
	string name;
	string nickname;
	bool isCivilian;
	uint8_t pad1[3];
	uint8_t unk1[0x40];
	// at 0x80
	// struct 0x15C bytes
	float storedEnergy;
	float storedMass;
	float incomeEnergy;    // div 10
	float incomeMass;      // div 10
	float reclaimedEnergy;
	float reclaimedMass;
	float requestedEnergy; // div 10
	float requestedMass;   // div 10
	float expenseEnergy;   // div 10
	float expenseMass;     // div 10
	// at 0xA8
	uint32_t maxEnergy;
	int unk2; // =0
	uint32_t maxMass;
	int unk3; // =0
	bool isResourceSharing;
	uint8_t pad2[7];
	// at 0xC0
	moho_set neutrals;
	moho_set allies;
	moho_set enemies;
	// at 0x120
	bool IsAlly;
	uint8_t pad3[7];
	moho_set mValidCommandSources;
	uint32_t color;
	uint32_t iconColor;
	string mArmyType; // 'human' for players
	// at 0x16C
	int faction;
	bool unk4;
	uint8_t pad4[7];
	uint32_t unk5[4];
	bool showScore;
	uint8_t pad5[7];
	uint32_t unk6;
	uint32_t pad6;
	uint32_t unk7;
	uint32_t pad7;
	uint32_t unk8[6];
	// at 0x1B8
	bool outOfGame;
	uint8_t pad8[3];
	Vector2f StartPosition;
	uint32_t unk9;
	float noRushRadius;
	float noRushOffsetX;
	float noRushOffsetY;
	float unk10;
	float unk11;
};
VALIDATE_SIZE(BaseArmy, 0x1DC);

struct UserArmy : BaseArmy
{	// 0x210 bytes
	// at 0x1DC
	uint8_t unk12[8];
	// at 0x1E4
	CWldSession *session;
	uint8_t unk13[0x28];
};
VALIDATE_SIZE(UserArmy, 0x210);

struct Sim;

struct SimArmy // : IArmy, BaseArmy
{//0x006FD332, 0x288 bytes
	void *vtable;
	// at 0xA4 in vtable
	//GetUnitCap;
	//SetUnitCap;

	void *unk1;
	// at 0x8, BaseArmy
	int armyIndex;
	string name;
	string nickname;
	// at 0x44
	bool isCivilian;
	uint8_t pad1[0x40];

	// at 0x88 Copy from [[self+1F4]+18]
	float storedEnergy;
	float storedMass;
	float incomeEnergy;    // div 10
	float incomeMass;      // div 10
	float reclaimedEnergy;
	float reclaimedMass;
	float requestedEnergy; // div 10
	float requestedMass;   // div 10
	float expenseEnergy;   // div 10
	float expenseMass;     // div 10

	uint32_t maxEnergy;
	int unk2; // =0
	uint32_t maxMass;
	int unk3; // =0
	bool isResourceSharing;
	uint8_t pad2[4];
	// at 0xC8
	moho_set neutrals;
	moho_set allies;
	moho_set enemies;
	// at 0x128
	bool IsAlly;
	uint8_t pad3[4];
	// at 0x130
	moho_set mValidCommandSources;
	// at 0x150
	uint32_t color;
	uint32_t iconColor;
	string mArmyType; // 'human' for players
	// at 0x174
	int faction;
	uint8_t pad4[0x48];
	// at 0x1C0
	bool outOfGame;
	// at 0x1C4
	Vector2f StartPosition;
	uint8_t pad5[0x4];
	// at 0x1D0
	float noRushRadius;
	float noRushOffsetX;
	float noRushOffsetY;
	uint8_t pad6[0xC];
	// at 0x1E8
	Sim *sim;
	void *CAiBrain;
	// at 0x1F0
	void *CAiReconDBImpl;
	SimArmyEconomyInfo *EconomyInfo;
	// at 0x1F8
	string unk4;
	uint8_t pad7[0x5C];
	// at 0x270
	float unitCap;
	uint8_t pad8[4];
	// at 0x278
	int pathCap_Land;
	int pathCap_Sea;
	int pathCap_Both;
	uint8_t pad9[4];
};
VALIDATE_SIZE(SimArmy, 0x288);

struct CArmyImpl : SimArmy
{//0x006FD332, 0x288 bytes
};

struct Entities
{	// 0x50 bytes
};

struct EntityChain // [[Entities+4]+4]
{
	void *next;
	//void *?;
	//void *?;
	int ID;
	void *entity;
};

struct Sim;
struct Entity : CScriptObject
{	// 0x270 bytes
	// at 0x68
	uint32_t EntityID; // For units x|xx|xxxxxx Type,Army,Num. Uses for UserSync
	REntityBlueprint *Blueprint;
	uint32_t CreationIndex; // ?
	// at 0x110
	bool VisibleAndControl;
};

struct Projectile : Entity
{//0x004C702B, 0x380 bytes
	// at 0x6C
	RProjectileBlueprint *Blueprint;
};

struct Prop : Entity
{//0x004C702B, 0x288 bytes
	// at 0x6C
	RPropBlueprint *Blueprint;
};

struct CUnitCommand
{	// 0x178 bytes
	void *vtable;
	void *unk1;
	void *unk2;
	LuaObject UserData;
	LuaObject Table;
	// at 0x4C
	float unk3;
	// at 0x5C
	float unk4;
	// at 0x60
	RUnitBlueprint* Build;
	string unk5;
	// at 0x98
	uint32_t Order;
	// at 0xA0
	uint32_t TargetID;
	Vector4f Pos1;
	// at 0x120
	Entity Target; //-0x4
	// at 0x128
	Vector3f Pos2;
	// at 0x160
	LuaObject Nil;
	void *unk6;
};

struct SCommand
{	// 0x8 bytes
	void *CUnitCommand; // +0x4
	void *nil;
};

struct Unit;
struct CommandQueue
{	// 0x28 bytes
	void *unk1;
	void *unk2;
	Unit *Owner;
	vector<SCommand> Commands;
	void *unk3;
	void *unk4;
	bool unk5;
};

struct UnitWeapon //: CScriptEvent
{//0x006D3114, 0x188 bytes
	void* vtable;
	// at 0x10
	void* vtable2;
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
	Unit* Owner;
};

struct CAiAttackerImpl // : IAiAttacker
{//0x005D6ABF, 0xA4 bytes
	void* vtable;
	// at 0x58
	vector<UnitWeapon*> Weapons;
};

struct UserUnitWeapon
{	// 0x98 bytes
	// at 0x54
	float MinRadius;
	float MaxRadius;
};

struct UnitIntel
{	// 0x20 bytes, AND 7FFFFFFF
	int VisionRadius;
	int WaterVisionRadius;
	int RadarRadius;
	int SonarRadius;
	int OmniRadius;
	int RadarStealthFieldRadius;
	int SonarStealthFieldRadius;
	int CloakFieldRadius;
};

struct Unit : WeakObject
{//0x006A5422, 0x6A8 bytes
	//WeakObject WeakObject;
	// at 0x8
	//Entity Entity; to 0x278
	// at 0x50
	void* self1;
	// at 0x70
	uint32_t UnitID;
	RUnitBlueprint *Blueprint;
	uint32_t CreationIndex; // ?
	void *unk1;
	void *unk2;
	void *RScmResource;
	void *RMeshBlueprint;
	Vector3f Scale; // at 0x8C
	float CurHealth;
	float MaxHealth;
	bool unk3;
	bool unk4;
	bool unk5;
	char pad1;
	Vector4f Rot1; // at 0xA4
	Vector3f Pos1;
	Vector4f Rot2;
	Vector4f Pos2;
	float FractionComplete; // at 0xE0
	void *unk6;
	char unk7[0x18];
	void *unk10;
	void *unk12;
	void *unk13;
	void *unk14;
	void *unk15;
	void *unk16;
	bool VisibleAndControl; // at 0x118
	char pad2[3];
	void *unk18;
	void *unk19;
	bool unk20;
	char pad3[3];
	void *unk21;
	UnitIntel UnitIntel; // at 0x130
	Sim *sim; // at 0x150
	SimArmy *owner;
	Vector4f Rot3;
	Vector3f Pos3;
	// at 0x17C
	int TickCount1; // Readonly
	void* CColPrimitiveBase;
	// at 0x248
	Vector3f Pos4;
	Vector3f Pos5;
	// at 0x294
	float FuelRatio;
	float ShieldRatio; // Readonly
	// at 0x2A0
	bool Paused;
	// at 0x2AC
	float WorkProgress;
	// at 0x2CC
	string customUnitName;
	// at 0x380
	UserUnitWeapon* Weapons;
	list<unk_t> unk22; // Weapons?
	void *unk23; // Weapons?
	// at 0x4B0
	void *MotionEngine; // +0xC FuelUseTime
	void *CommandQueue;
	int Enum; //0..4
	// at 0x534
	void *workValues; // +0x8
	bool Flag;
	// at 0x53C
	float WorkRate;
	// at 0x544
	void* IAiAttacker;
	// at 0x55C
	void* IAiTransport;
	// at 0x59C
	Vector3f Pos6;
	// at 0x668
	int tickCount2; // Readonly
	// at 0x68E
	bool updWeaponRadius;
};

struct UserEntity : WeakObject
{	// 0x148 bytes
	// at 0x44
	int entityID;
	RPropBlueprint *blueprint;
	// at 0x58
	RMeshBlueprint *mesh;
	// at 0x68
	float curHealth;
	float maxHealth;
	// at 0x70
	bool isBeingBuilt;
	Vector4f rot1;
	Vector3f pos1;
	// at 0x90
	Vector4f rot2;
	Vector4f pos2;
	// at 0xB0
	float fractionComplete;
	// at 0xD0
	//float x1,y1,x2,y2;
	// at 0x100
	UnitIntel unitIntel;
	UserArmy *owner; // at 0x120
	Vector4f rot3;
	Vector4f pos3;
};

struct UserUnit : UserEntity
{//0x008B8601, 0x3E8 bytes
	using Type = ObjectType<0x10C77AC, 0xF881E0>;
	// at 0x44
	uint32_t UnitID;
	RUnitBlueprint *blueprint;
	// at 0x1B0
	bool Paused;
	// at 0x1BC
	float WorkProgress;
	// at 0x1DC
	string customUnitName;
	// at 0x290
	UserUnitWeapon *weapons;
};

struct CPlatoon : public CScriptObject
{
	using Type = ObjectType<0x10C6FCC, 0xF6A1FC>;
};

struct CMauiBitmap : public CMauiControl
{
	using Type = ObjectType<0x10C7704, 0xF832F4>;
};

struct ReconBlip : Entity
{	// 0x4D0 bytes
	Entity entity;
	// at 0x270
	void *originUnit; // -0x4
	// at 0x28C
	void *StatItem;
	void *StatItem2;
	// at 0x320
	void *CAniPose;
	// at 0x328
	void *CAniPose2;
	// at 0x330, size 0x30?
	list<unk_t> unk1;
	void *unk2;
	// at 0x360, size 0x30?
	list<unk_t> unk3;
	void *unk4;
	// at 0x390, size 0x30?
	list<unk_t> unk5; // Weapons?
	void *unk6; // Weapons?
	// at 0x450, size 0x30?
	list<unk_t> unk7;
	void *unk8;
	// at 0x4C4
	void *armyesData[]; //size 0x34
};

struct STIMap;
struct Sim // : ICommandSink
{	// 0xAF8 bytes
	void *vtable;
	uint8_t pad1[0x4C];
	// at 0x50
	char dynamicHash[16];
	char hashTrash[0x50];
	char simHashes[16*128]; // at 0xB0-8B0
	uint8_t pad2[0x10];
	// at 0x8C0
	void *CEffectManager;	// 0x18 bytes
	void *CSimSoundManager; // 0x720 bytes
	RRuleGameRules *rules;	// From CSimDriver.LaunchInfoNew
	STIMap *STIMap;		// From CSimDriver.LaunchInfoNew
	CSimResources *res;
	uint8_t pad3[4];
	// at 0x8D8
	LuaState *LState;
	uint8_t pad4[0xA];
	// at 0x8E6
	bool cheatsEnabled;
	uint8_t pad5[0x10];
	// at 0x8F8
	uint32_t beatCounter1;
	void *unk1; // self+0x900 or null
	uint32_t beatCounter2;
	// at 0x904
	void *unk2; // 0x9CC bytes
	void *unk3; // 0x68 bytes
	vector<SimArmy*> armies;
	uint8_t pad6[4];
	// at 0x920
	list<SSTICommandSource> cmdSources;
	// at 0x92C
	int ourCmdSource; // Possibly just current in simulation.
	uint8_t pad7[0x4C];
	// at 0x97C
	void **unk4; // 0x30 bytes
	void *CAiFormationDB; // 0x40 bytes
	// at 0x984
	void *Entities;
	void *unk5; // 0xCD0 bytes
	uint8_t pad8[0x10];
	// at 0x99C
	void *unk6; // 0xCF0 bytes
	uint8_t pad9[0x98];
	// at 0xA38
	void *unk7; // 0xC bytes
	uint8_t pad10[0x4C];
	// at 0xA88
	int focusArmyIndex; // Focused army, -1 = observer
	uint8_t pad11[0x6C];
};
VALIDATE_SIZE(Sim, 0xAF8)

struct CWldSession
{//0x0089318A, 0x508 bytes
	CWldSession *self1;
	CWldSession *self2;
	void *self_weird1; // this + 0x8
	void *self_weird2; // this + 0x8
	// at 0x10
	LuaState *LState; // Set from constructor argument
	void *unk1; // 0x14 bytes
	RRuleGameRules *rules;
	CWldMap *map;
	LaunchInfoNew *launchInfo;
	void *unk2; //class detail::boost::sp_counted_impl_p<struct Moho::LaunchInfoNew>
	string mapName;
	Entities entities;
	uint8_t pad1[0x3A8];
	// at 0x3F0
	list<UserArmy*> armies;
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
	struct {
		int unk1;
		void **SelList; //+0x10
		int SelCount;
		int SelCount2;
	} selectedUnits;
	// at 0x4B0
	struct
	{	// size 0x24
		void *unk1;
		float mouseWorldPosX;
		float mouseWorldPosY;
		float mouseWorldPosZ;
		void *unk2;
		void *unk3;
		int IsDragger;
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
VALIDATE_SIZE(CWldSession, 0x508)

struct CSimDriver // : ISTIDriver
{//0x0073B59E, 0x230 bytes
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

struct CHeightField // : class detail::boost::sp_counted_base
{//0x00579121, 0x10 bytes
	void* vtable;
};

struct MapData
{	// 0x1C bytes
	uint32_t *TerrainHeights; // Word(TerrainHeights+(Y*SizeX+X)*2)
	int SizeX; // +1
	int SizeY; // +1
};

struct STIMap
{	// 0x1548 bytes
	MapData *MapData;
	CHeightField *HeightField;
	uint32_t unk1[4];
	// at 0x18
	void *beginData;
	void *endData;
	void *endData2;
	void *beginData2;
	// at 0x28
	LuaObject Data[0x100]; // Type desc tables
	uint8_t *TerrainTypes; // TerrainTypes+(Y*SizeX+X)
	int SizeX;
	int SizeY;
	uint8_t unk2[0x100];
	// at 0x1534
	BOOL Water;
	float WaterLevel;
	float DepthLevel;
	float AbyssLevel;
	uint32_t unk3;
};
VALIDATE_SIZE(STIMap, 0x1548);

struct CPushTask
{
	char data[0x24];
};

typedef CPushTask CPullTask;

struct INetConnector
{
	void* vtable;
};
struct CLobby
{//0x004C702B, 0xC8 bytes
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
	void* not_host; // 0 if we're host

	// at 0x90
	string our_name;
	// at 0xAC
	int our_UID;

	// at 0xB0
	struct
	{
		INetConnector* first_element;
		INetConnector* last_element; // It will probably always be our CNetXXXConnector
	} peer_list; // Probably singly-linked list
};

struct sub_10392B10_ret
{	// 0x20 bytes

	void* zero1;
	void* zero2;
	void* zero3;
	int unk1;

	// at 0x10
	bool one1;
	bool zero4;
};

struct CLobbyPeer
{	// 0x50 bytes

	CLobbyPeer* next; // Doubly linked list pointers
	CLobbyPeer* prev;

	string playerName;

	// at 0x24
	int ownerID;	// User UID
	int const1;
	short const2;
	char pad1[2];

	// at 0x30
	int const3;	// enum?
	float const4;	// = 0
	int const5;
	int unk1;

	// at 0x40
	sub_10392B10_ret* unk2; // made in sub_10394180
	int zero1;
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
{	// 0x184D0 bytes
	// at 0x40C
	gpg_mutex mLock;

	// at 0x420
	list<IClient*> mClients;
	INetConnector* mConnector;

	// at 0x434
	bool mWeAreReady;
	bool mEveryoneIsReady;
	char pad1[2];
	int mDispatchedBeat;
	int mAvailableBeat;
	int mFullyQueuedBeat;
	int mPartiallyQueuedBeat;
	int gameSpeedChangeCounter; // mGameSpeedClock
	int mGameSpeedRequester;    // Always 0
	int gameSpeedRequested;     // mGameSpeed
	bool speedControlEnabled;
	// at 0x458
	uint32_t hEvent; // for KERNEL32.SetEvent
	// at 0x18470
	PipeStream mPipe;
	// at 0x184BC
	bool unk1; // if value is 1 then KERNEL32.SetEvent is bypassed
};

typedef struct mRequest {IClient* mRequester; int mAfterBeat;};

struct CClientBase : IClient
{//0x0053B5E9, 0xD8 bytes
	string mNickname;
	// at 0x20
	int mIndex; // client index
	int mUID;
	IClientManager* clientManager;

	// at 0x30
	moho_set unk1;
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
	vector<mRequest> mEjectRequests;
	int maxSimRate; // from CalcMaxSimRate
};

struct CLocalClient : CClientBase
{//0x0053B5E9, 0xD8 bytes
};	// equal CClientBase

struct CReplayClient : CClientBase
{	// 0x160 bytes
	// before 0xD8 it CClientBase
	// at 0xD8
	FileStream* stream;
	// at 0xE0
	void* ptr1; // self+0xF0
	void* ptr2; // self+0xF0
	void* ptr3; // self+0x130
	void* ptr4; // self+0xF0
	void* unk1;
	// at 0x130
	void* unk2;
	// at 0x138
	uint32_t replayBeat;
	bool unk3;
	void* unk4;
	uint32_t hSemaphore1;
	uint32_t hSemaphore2;
	uint32_t hMutex;
	// at 0x15C
	bool unk5;
	bool unk6;
};

struct CNetUDPConnection {};

struct CNetUDPConnetor // : INetConnector
{	// 0x18090 bytes
	void* vtable;
	void* smth; // Listen socket fd?
	gpg_mutex mMutex;
	// at 0x14
	SOCKET mSocket;
	// at 0x24
	linked_list<CNetUDPConnection*> mConnections;
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
{	// 0x14 bytes
	void* vtable;
};

struct MeshThumbnailRenderer
{	// 0x3C bytes
	void* vtable;
};

struct RangeRenderer
{	// 0x94 bytes
	void* vtable;
};

struct VisionRenderer
{	// 0x78 bytes
	void* vtable;
};

struct BoundaryRenderer
{	// 0x68 bytes
	void* vtable;
};

struct Shadow
{	// 0x318 bytes
	void* vtable;
};

struct Clutter
{	// 0x192C bytes
	void* vtable;
};

struct Silhouette
{	// 0x74? bytes
	void* vtable;
};

struct WRenViewport // : WD3DViewport
{//0x0097977D, 0x21A8 bytes
	void* vtable;
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