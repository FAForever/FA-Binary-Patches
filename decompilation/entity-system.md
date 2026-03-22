# Entity & Unit System – Decompilation Notes

*ForgedAlliance.exe v3599 @ 0x00400000*

## Key Findings Summary

All class names are in the `Moho::` namespace. Unit tasks, AI brain, and weapons are exposed to Lua via a **Lua binding registration system** at `DAT_00f5a124` – these are NOT directly callable as `__thiscall` from DLL code.

---

## 1. CUnitMoveTask (`0x00618700` = destructor)

The unit move task is the core of pathfinding-driven movement.

### Struct Layout (from `CUnitMoveTask::~CUnitMoveTask`)

```
CUnitMoveTask (param_1 = this pointer):
  +0x000  vtable ptr               -> Moho::CUnitMoveTask::vftable
  +0x034  vtable ptr (copy)        -> Moho::CUnitMoveTask::vftable  [+0x0D*4]
  +0x044  vtable ptr               -> Moho::CUnitMoveTask::vftable  [+0x11*4]
  +0x054  vtable ptr               -> Moho::CUnitMoveTask::vftable  [+0x15*4]
  +0x01C  mUnit ptr                -> CUnit*  (task[7])
  +0x038  Listener<EAiNavigatorEvent>::vftable  (task[0xD])
  +0x044  Listener<EFormationdStatus>::vftable  (task[0x11])
  +0x054  Listener<ECommandEvent>::vftable      (task[0x15])
  +0x058  linked list node (task[0x16..0x17])
  +0x091  bool inFormation         (char, task byte 0x91)
  +0x092  bool pathfindingActive   (char, task byte 0x92)
  +0x090  pathfinder ref           (task[0x22] = ref-counted ptr)
  +0x093  bool canFireOnMove       (task byte 0x24)
```

### CUnit fields accessed by CUnitMoveTask (mUnit = task[7])

```
CUnit:
  +0x070  position ptr / display info
  +0x4a0  unit flags          (bitmask; bit 2 = 0x04 cleared on move end)
  +0x4a4  unit flags2
  +0x4b4  active navigator    -> CAiNavigator* (pathfinder result)
  +0x4c8  command list head   -> linked list of commands
  +0x4cc  command list tail
  +0x54c  formation ptr       -> formation object (nullable)
```

### Key functions
| Address      | Function                              |
|--------------|---------------------------------------|
| `0x00618700` | `CUnitMoveTask::~CUnitMoveTask()`     |
| `0x00618f10` | `CUnitMoveTask::GetName()` → `"CUnitMoveTask"` |
| `0x00608e90` | base task destructor (called at end)  |
| `0x006187d7` | prints `mUnit=0x%08x` via `Sim_LogPrintf` |
| `0x006189c0` | check function (returns bool, pathfind validity) |
| `0x006edc20` | navigator status check                |

---

## 2. CAiPathFinder

RTTI present: `Moho::CAiPathFinder` @ `0x00F6BABC`

### RTTI chain
```
".?AVCAiPathFinder@Moho@@"              00f6babc
".?AUCAiPathFinderTypeInfo@Moho@@"      00f6b964
".?AUCAiPathFinderSerializer@Moho@@"    00f6b938
".?AU?$SerSaveLoadHelper@VCAiPathFinder@Moho@@@gpg@@"  00f6b8fc
```

### Name function
| Address      | Function |
|--------------|----------|
| `0x005aab50` | `CAiPathFinder::GetName()` → `"CAiPathFinder"` |

The pathfinder is stored as `CAiNavigator*` at `CUnit+0x4b4`. The task uses `Listener<EAiNavigatorEvent>` to receive path completion callbacks.

> **Correction (v22):** The path from `CAiSteeringImpl` to `CUnit*`
> does NOT go through `CAiNavigatorImpl+0x5C`. 
> `CAiSteeringImpl+0x1C` is a **direct `CUnit*`** pointer.
> The `CAiNavigatorImpl+0x5C` back-pointer is only relevant
> when navigating FROM the navigator TO the unit, not from steering.

---

## 3. CAiBrain

RTTI: `"CAiBrain"` @ `0x00E19738`

### Lua binding registration (60+ functions)
All bindings register into linked list at `DAT_00f5a124` via constructor calls.
Pattern: each FUN registers one Lua method with signature string.

| Address      | Lua Method                    | Signature |
|--------------|-------------------------------|-----------|
| `0x00579ba0` | `CAiBrain::GetName()`         | → `"CAiBrain"` |
| `0x00585f10` | `IsOpponentAIRunning`         | `"Returns true if opponent AI should be running"` |
| `0x0058a460` | `FindPlaceToBuild`            | `"brain:FindPlaceToBuild(type, structureName, buildingTypes, relative, builder, ..."` |
| `0x00586090` | listed                        | (more below) |
| `0x0057c3d3` | `PickBestAttackVector` catch  | error handler |

Strings found (Lua binding names):
```
CAiBrain:PlatoonExists()                @ 0x00e1a6d0
CAiBrain:GetPlatoonsList()              @ 0x00e1a6fc
CAiBrain:DisbandPlatoon()               @ 0x00e1a728
CAiBrain:DisbandPlatoonUniquelyNamed()  @ 0x00e1a754
CAiBrain:MakePlatoon()                  @ 0x00e1a798
CAiBrain:AssignUnitsToPlatoon()         @ 0x00e1a7bc
CAiBrain:GetPlatoonUniquelyNamed()      @ 0x00e1a80c
CAiBrain:GetNumUnitsAroundPoint()       @ 0x00e1a848
CAiBrain:GetUnitsAroundPoint()          @ 0x00e1a884
CAiBrain:FindClosestArmyWithBase()      @ 0x00e1a8b8
CAiBrain:SetUpAttackVectorsToArmy()     @ 0x00e1a8f4
```

> **Key insight**: CAiBrain is NOT a standalone singleton. It is fetched per-army via the Lua subsystem. Each army has its own CAiBrain instance. The brain operates on platoons, not individual units.

---

## 4. CWeaponAttributes

RTTI: `"CWeaponAttributes"` @ `0x00E2D8C0`

```
".?AVCWeaponAttributes@Moho@@"          00f7bcf0
".?AUCWeaponAttributesTypeInfo@Moho@@"  00f7bbb8
".?AUCWeaponAttributesSerializer@Moho@@" 00f7bb88
```

> **TODO**: Decompile constructor to extract weapon attribute fields (damage, range, reload time, etc.)

---

## 5. Unit Task Classes Found

From string scan:

| Class                    | String Address |
|--------------------------|----------------|
| `CUnitAssistMoveTask`    | `0x00e1f4d8`  |
| `CUnitAttackTargetTask`  | `0x00e1f544`  |
| `CUnitMobileBuildTask`   | `0x00e1f804`  |
| `CUnitUpgradeTask`       | `0x00e1f8c4`  |
| `CUnitRepairTask`        | `0x00e1f8f0`  |
| `CUnitSacrificeTask`     | `0x00e1f930`  |
| `CUnitCallTransport`     | `0x00e1fb90`  |
| `CUnitCaptureTask`       | `0x00e1fe00`  |
| `CUnitCarrierRetrieve`   | `0x00e1ffc0`  |
| `CUnitCarrierLand`       | `0x00e1ffe8`  |
| `CUnitCarrierLaunch`     | `0x00e20024`  |
| `CUnitGetBuiltTask`      | `0x00e201c4`  |
| `CUnitTeleportTask`      | `0x00e201d8`  |
| `CUnitFireAtTask`        | `0x00e20210`  |
| `CUnitFerryTask`         | `0x00e203a0`  |
| `CUnitGuardTask`         | `0x00e20474`  |
| `CUnitMeleeAttackTargetTask` | `0x00e2051c` |
| `CUnitMoveTask`          | `0x00e20600`  |
| `CUnitFormAndMoveTask`   | `0x00e20610`  |
| `CUnitPatrolTask`        | `0x00e20704`  |
| `CUnitPodAssist`         | `0x00e207a0`  |
| `CUnitReclaimTask`       | `0x00e20844`  |
| `CUnitRefuel`            | `0x00e20988`  |
| `CUnitScriptTask`        | `0x00e20ad8`  |

---

## 6. Lua Binding Registration Pattern

All Moho Lua bindings follow this pattern (from CAiBrain Lua funcs):

```cpp
void RegisterBinding_FUN_0058XXXX(void) {
    _DAT_010aXXXX_funcptr = DAT_00f5a124;  // link into chain
    _DAT_010aXXXX_name    = "FunctionName";
    _DAT_010aXXXX_class   = "CAiBrain";
    DAT_010aXXXX_sig      = "signature string";   // Lua docs string
    DAT_00f5a124          = &DAT_010aXXXX;        // update chain head
    _DAT_010aXXXX_impl    = &LAB_impl;
    _DAT_010aXXXX_vtbl    = &PTR_vftable_00f8d6cc; // LuaFuncDef vtable
    _DAT_010aXXXX_self    = LuaFuncDef::vftable;
}
```

**Implication**: These cannot be called directly as C++ functions from a DLL. To invoke game logic from injection code, the correct approach is:
1. Find the actual C++ implementation called INSIDE the binding function
2. Call that implementation directly with the correct `this` pointer

---

## Next Steps

- [ ] Decompile `CUnitMoveTask` constructor to find full struct size
- [ ] Decompile `CUnitAttackTargetTask` to find weapon/target fields
- [ ] Decompile `CAiBrain::PickBestAttackVector` (main function at ? – find via catch at `0x57c3d3`)
- [ ] Find `CUnit` constructor to map full unit struct layout
- [ ] Decompile `CWeaponAttributes` constructor for weapon stat fields
- [ ] Find `CAiNavigator::FindPath` for pathfinding algorithm details
