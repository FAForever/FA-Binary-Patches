#pragma once

#include <cstdint>
#include "../../workflow.cpp"
/*
GetClassNamePtr -> [[[Self]-4]+C]+8
GetParentClassNamePtr -> [[[[Self]-4]+24+1*4]]+8

TrueDataOffset -> -[[[Self]-4]+4]
ClassAncestors -> [[Self]-4]+24

Get debugging info about a Lua call:

[[[lua_State+28]+lvl*5*8]-4] = FuncPtr

[[FuncPtr+18]+20]+14 = full path to file

[[FuncPtr+18]+3C] = line defined

[lua_State+28]+lvl*5*8 = CallInfo //lua.org/source/5.0/lstate.h.html#CallInfo
[[[CallInfo]-4]+18] = Proto       //lua.org/source/5.0/lobject.h.html#Proto
[[CallInfo+0C]-[Proto+0C]+[Proto+14]] = currentline
*/
#define GDecl(addr, type) \
  (*(type*)addr)

#define WDecl(addr, type) \
  ((type)*(uintptr_t*)addr)

#define FDecl(addr, name, type) \
  const auto name = (type)addr;

#define VALIDATE_SIZE(struc, size) \
  static_assert(sizeof(struc) == size, "Invalid structure size of " #struc);

#define g_CSimDriver			GDecl(0x10C4F50, CSimDriver*)
#define g_SWldSessionInfo		GDecl(0x10C4F58, SWldSessionInfo*)
#define g_CWldSession			GDecl(0x10A6470, CWldSession*)
#define g_Sim				GDecl(0x10A63F0, Sim*)
#define g_EntityCategoryTypeInfo	GDecl(0x10C6E70, EntityCategoryTypeInfo*)
#define g_CAiBrainTypeInfo		GDecl(0x10C6FA0, CAiBrainTypeInfo*)
#define g_CUIManager			GDecl(0x10A6450, CUIManager*)
#define g_EngineStats			GDecl(0x10A67B8, EngineStats*)
#define g_WRenViewport			GDecl(0x10C7C28, WRenViewport*)
#define g_ConsoleLuaState		GDecl(0x10A6478, LuaState*)
#define g_Device			GDecl(0x0F8E284, Device*)

#define ui_ProgressBarColor		GDecl(0x0F57BB8, int)
#define ui_SelectTolerance		GDecl(0x0F57A90, float)
#define ui_ExtractSnapTolerance		GDecl(0x0F57A94, float)
#define ui_DisableCursorFixing		GDecl(0x10A6464, bool)
#define ui_RenderIcons			GDecl(0x0F57B27, bool)
#define range_RenderSelected		GDecl(0x10A640A, bool)
#define range_RenderHighlighted		GDecl(0x10A640B, bool)
#define range_RenderBuild		GDecl(0x10A6414, bool)
#define d3d_WindowsCursor		GDecl(0x10A636E, bool)
#define debugSelect			GDecl(0x10A645E, bool)

#define s_FACTORY			GDecl(0xE19824, const char*)
#define s_EXPERIMENTAL			GDecl(0xE204B8, const char*)
#define s_global			GDecl(0xE00D90, const char*) // "<global>"

#define g_ExeVersion1			GDecl(0x876666, const int)
#define g_ExeVersion2			GDecl(0x87612d, const int)
#define g_ExeVersion3			GDecl(0x4d3d40, const int)

FDecl(0x9C4940, AbortF, void (*)(wchar_t* format, ...))
FDecl(0x937CB0, LogF,		int (*)(const char *fmt, ...))
FDecl(0x937D30, WarningF,	int (*)(const char *fmt, ...))
FDecl(0x937C30, SpewF,		int (*)(const char *fmt, ...))
FDecl(0x41C990, ConsoleLogF,	int (*)(const char *fmt, ...))
FDecl(0xA9B4E6, FileWrite,	int (*)(int fileIndex, const char *str, int strlen)) //index 3 is log.
FDecl(0xA825B9, shi_new,	void* (*)(size_t size))
FDecl(0x957B00, realloc,	void* (*)(void *ptr, size_t new_size))
FDecl(0x958B20, malloc,		void* (*)(size_t size))
FDecl(0x958C40, free,		void (*)(void *ptr))
FDecl(0x957EA0, msize,		size_t (*)(void *memblock))
FDecl(0x957AB0, calloc,		void* (*)(size_t num, size_t size))
FDecl(0xA89110, memset,		void* (*)(void *dest, int ch, size_t count))
FDecl(0xA89190, memcpy,		void* (*)(void *dest, const void *src, size_t count))
FDecl(0x452FC0, sqrtf,		float (*)(float arg))
FDecl(0xA94450, strlen,		size_t (*)(const char *str))
FDecl(0xAA549E, strcmp,   int (*)(const char *str1, const char *str2))
FDecl(0xA82F32, sprintf_s, int (*)(char *const Buffer, const size_t BufferCount, const char *const Format, ...))

FDecl(0x405550, InitString,	__thiscall void (*)(void *this_, const char *str))
FDecl(0x4059E0, AssignString,	__thiscall void (*)(void *this_, const char *str, size_t size))

#define GetModuleHandle WDecl(0xC0F378, __stdcall void* (*)(const char *lpLibFileName))
#define GetProcAddress  WDecl(0xC0F48C, __stdcall void* (*)(void* hModule, const char *lpProcName))

#define QueryPerformanceCounter   WDecl(0xC0F470, __stdcall bool (*)(int64_t*))
#define QueryPerformanceFrequency WDecl(0xC0F46C, __stdcall bool (*)(int64_t*))

/*
LuaPlus: See FALuaFuncs.txt

Use Tables example:
CreateTable
  CreateTable
    PushNumber
    PushNumber
    PushNumber
    PushStack
    PushTable
  LuaObjectFinalize
  PushStack
LuaObjectFinalize

//Lua internals. For debug only.

009133A0 luaG_typeerror
004154B0 LuaStackObjectTypeError
009134B0 errorfb
00457880 luaplus_assert
009274D0 luaH_getstr
0091A240 luaM_realloc
00927610 luaH_index
00914E90 luaF_newCclosure(lua_State*, int numArgs)
00913990 luaD_growstack(lua_State*, numArgs)
00915D90 luaC_collectgarbage
009248E0 CreateHashStr
009142A0 CallCFunctionFromLua
009240A0 GetLuaState(lua_State*):eax
0090A510 GetLuaState+1

// Other

00938E00 Format
00938F10 Format+1
00A89950 CxxThrowException

00957A70 malloc+1
00A82130 shi_new+1
00957AF0 free+1
00957A60 free+2
00A82542 free+3

00459D10 OpenFile
008E0750 LookupRType
004CCE70 CScrLuaObjectFactory::Get
004D26D0 Moho::SCR_Copy //Copy lua object
00529F70 RRuleGameRulesImpl::ExportToLuaState
004CE020 DoFile(LuaObject* ecx, char* file, Table* env):al bool
00908A70 GetVar(out LuaObject* ecx, LuaState**):eax LuaObject
009132F0 lua_getinfo
00AA549E int StrCmp(char* str1, char* str2)
00A89190 CopyMem(void* dest, void* src, len)
006E5660 Moho_SetSetSize(moho_set* ecx, setSize)
004036A0 Moho_SetAdd(moho_set* edi, setSize esi, out 0xC)
009072A0 NullifyBytes16(ecx)
00A89CC0 FloatToInt(value):eax
004035F0 IsValidCommandSource
00408450 CompareStrings(char* str1, char* str2, strLen1):eax -1,0,1
0040A880 CompareStrings2(char* str1, strLen1, char* str2, strLen2):eax
0041CC90 SimConExecute
004CD3A0 Register LUA CFunction
00528460 RRuleGameRulesAlloc(Arg1,Arg2)
00529120 RRuleGameRulesInit(Arg1,Arg2,Arg3)
00529510 DestroyRRuleGameRules
00546650 FindRes(CSimResources* ecx, Type, Rect*):Bool
00546760 FindRes(CSimResources* ecx, PtrPosXY, PtrResultXY, Radius, Type):Bool
00581AA0 CallFunctionLua(self ecx)
005BD630 Internal IsAlly
005D5540 Internal IsEnemy
00707BF0 Internal IsNeutral
006FE530 SimArmyAlloc(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)
006FE690 SimArmyCreate(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)
006FE670 DestroySimArmy
004C8270 Get_c_object //From table Ptr to value
005936C0 GetSimCUnit(Table* eax):eax
005E3800 GetCUnit(Table* eax):eax
005930D0 GetCBrain(Table* eax):eax
00707D60 GetSimArmy
0073B1B0 SetArmyIndex(CSimDriver* ecx, value)
0073F4E0 CSimDriverAlloc(Arg1,Arg2,Arg3,Arg4)
0073B570 CSimDriverInit(Arg1,Arg2,Arg3)
007433B0 SimAlloc
007434D0 SimCreate(Arg1,Arg2)
00744060 SimSetup(Arg1,Arg2)
007458E0 SimFinalize(Sim*)
006A9F40 UpdCompleteProgress
00747180 IsCheating!
005459F0 GetLuaNumber
0067AFF0 LayerChange
007A4970 OnMouseMove
008704B0 OnMouseMoveOverMap
0086E480 CUIWorldViewInit(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)
00885DE0 WorldSessionLoad(Arg1)
0088C000 DoLoading
00897220 CWldSessionAlloc
00893160 CWldSessionCreate(Arg1,Arg2,Arg3)
008965E0 Internal SetFocusArmy
00896670 Test Set focus access rights
008C0500 SelectUnit
008CEDE0 AppInit
008D4010 SC_CreateEntityDialog
0055AE10 CalcMaxSimRate
0053E590 SetSimRate
0053E720 GetSimRate
0053E7E0 GetSimRateRequested
007EF9B0 DrawRings
0081C660 DrawVision
008F3C40 D3DXEffect::GetPassCount ?
008F4080 D3DXEffect::BeginPass ?
008F4260 D3DXEffect::EndPass ?
00941D70 D3DXEffect::SetMatrix ?
00941F60 D3DXEffect::SetTechnique
00943A90 EffectVariableD3D9::SetMem(int len, float *ptr)
00943E10 EffectVariableD3D9::SetMatrix4x4(float *ptr)
00943550 EffectVariableD3D9::SetFloat(float *ptr)
00452AF0 Vec3NormInPlace(float* ecx)
008F5950 LockVertexBuffer
008F5B40 UnlockVertexBuffer
0081F7B0 GetLeftMouseButtonAction
0081EC00 GetRightMouseButtonAction
008B43F0 GetEntitiesUnderCursor ?
00858D80 DisplayEconomyOverlay
00430590 D3DGetDevice
008E6730 GetDeviceD3D9
00430590 GetCD3DDeviceVTbl**
008D82F0 CreateBitArray2D
008D8210 BitArray2DReset
008D8200 DestroyBitArray2D
005C36A0 CreateCAiReconDBImpl
005BFF90 InitCAiReconDBImpl
00507720 InitCIntelGrid
0067AE70 SetEntityCollision
0067AD30 CreateCollisionSphere
0067AC40 CreateCollisionBox
0067AE00 DestroyCollision
0084C5E0 CreateCUIManager
0084C9C0 InitCUIManager
00408940 CreateEngineStats
004088C0 InitEngineStats(EngineStats*)
0040C200 GetEngineStatsItem(EngineStats* ecx, char* pathAndName, int value):eax
0040A0A0 CreateStatItemRoot(EngineStats*):eax
00408730 InitStatItem(StatItem*, char* name):eax
008E5050 CalcHash
005D62B0 CreateCAiAttackerImpl
005D6AA0 InitCAiAttackerImpl
005D5D90 AiAttacker::GetWeaponCount
005BE6E0 InitReconBlip
00542870 CreateLaunchInfoNew
00542790 InitLaunchInfoNew
005427F0 DestroyLaunchInfoNew
0088C9D0 CreateSWldSessionInfo
0088D060 InitSWldSessionInfo
0053E180 CreateCLocalClient
0053BD40 InitCClientBase
0053FAF0 CreateCClientManager
0053DF20 InitCClientManager
0053E400 CreateCReplayClient
0053BA50 InitCReplayClient
00956DB0 InitStream
009565D0 InitPipeStream
00955BD0 InitFileStream
00955870 DestroyFileStream
0048BBE0 CreateCNetUDPConnetor
004896F0 InitCNetUDPConnetor
004899E0 DestroyCNetUDPConnetor
007AA9C0 CreateCamera
007A7950 InitCamera
007A7DC0 DestroyCamera
0086E060 InitCameraDragger
00749F40 SimBeat
0073DAD0 SimSync
00894530 UserSync
008B1520 InitUserArmy
008B85E0 InitUserEntity
008BF420 InitUserUnit
008B8760 DestroyUserEntity
008BF990 DestroyUserUnit
004C7010 InitRObjectAndCScriptObject
00677C90 InitEntity
007489E0 CreateUnit
006A53F0 InitUnit
006A5320 DestroyUnit
006A0FB0 CreateProjectile
0069AFE0 InitProjectile
006FB3B0 CreateProp
006F9D90 InitProp
0050DD60 InitRBlueprint
00511C30 InitREntityBlueprint
00532680 CreateRMeshBlueprint
005283A0 InitRMeshBlueprint
00531D80 CreateRUnitBlueprint
0051E480 InitRUnitBlueprint
00532080 CreateRPropBlueprint
0051D250 InitRPropBlueprint
00532380 CreateRProjectileBlueprint
0051B740 InitRProjectileBlueprint
005289D0 RegisterBlueprint(RRuleGameRules*, char* Category)
007FA230 CreateWRenViewport
007F66A0 InitWRenViewport
007F90D0 TerrainRender
008015C0 SetTerrainVariables
007D2EA0 SetCartographicVariables
007E19D0 SetMeshVariables
007EDFE0 GenerateRingCylinders
007EF5A0 RenderRings
005779C0 CreateMapData
004783D0 CreateTerrainHeights
00577890 InitSTIMap
005790E0 CreateCHeightField
0044FB90 GetTerrainHeight(MapData*, float x, float z):int ecx
00758E10 GetTerrainType(STIMap* eax, int esi X, int edi Z):LuaObject ebx
0074B120 FlattenTerrain(STIMap*, int Rect*, float Height)
00476BB0 UpdateMinimap(MapData*, int x1, int y1, int x2, int y2)
00890DA0 MapLoad
0057CBB0 CanBuildStructureAt(CAiBrain*, x, y, z, Blueprint*, ?, ?, ?):al
006856C0 SimFindEntityChainById(ecx* entities, ebx* id, eax* result)
00898DC0 UserFindEntityChainById(ecx* entities, ebx* id, eax* result)
008B05E0 ISSUE_Command
008B0180 GiveOrder(list<UserUnit*>*, unused void*, out int?, struct?)

const int LuaStackObject__GetBoolean = 0x415560;

const int _AssignClientIndex_CLobby_Moho__AAEXHVStrArg_gpg__AAIAAH_Z = 0x7C4E80;
const int _AssignCommandSource_CLobby_vector_allocator_USSTICommandSource = 0x7C4F60;

const int lua_cast_CLobby_ = 0x7CB7E0;

const int Moho__NET_MakeNATTraversal_ = 0x7BC5F0; //(LuaObject* rvo, LuaState*)
const int _Eval_Lua_Object = 0x6EEE40;

const int __imp_atoi = 0xA835CE;

const int _Validate_IssueCommand = 0x006EECF0;
const int _Get_Lua_Coordinates_State = 0x006EEF60;
const int _Push_Coordinates = 0x005E27D0;
const int _Moho_UNIT_IssueCommand = 0x006F12C0;
const int _Moho_SSTICommandIssueData_Destructor = 0x0057ABB0;

#define _CannotQueCommandInConstruct = 0x006EFB0E;
#define _CanQueCommandInConstruct = 0x006EFAF8;
#define _NeitherInCategoryInConstruct = 0x006EFACE;
#define _EndCalculateNoRushTimerVariable = 0x006FF3D6;

#define _GetCatCmpResult = 0x0067B050;
#define _Moho_SSTICommandIssueData_SSTICommandIssueData = 0x00552550;

// MSVCR80.dll
#define _memmove_s = 0x00A824E7;

// New unit categories.
const char* sCQUEMOV = "CQUEMOV";
*/