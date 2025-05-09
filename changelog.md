# Changelog

All notable changes to this project will be documented in this file. Unlike other changelogs, this particular changelog is not in chronological order. Instead the changes that are similar are grouped together.

## Signature patches

- See `SigPatches.txt`

## Technical patches

These don't matter except for other assembly patches

- Adds `lua_createtable`

  - section/lua_createtable.cpp

- Adds utilities to register Sim and UI Lua functions.

  - hooks/LuaFuncRegs.cpp
  - section/LuaFuncRegs.cpp

## Optimizations

- Optimize `VDist3` function

  - hooks/VDist3.cpp
  - section/VDist3.cpp

- Reduce call sim beat of UI

  - hooks/HUIReduceCallSimBeat.cpp

- Zeroing GuardScanRadius if the unit's order is not Patrol/AttackMove. Increases performance.

  - hooks/HGuardFix.cpp
  - section/GuardFix.cpp

- Range ring performance improvement (reducing height cylinders)

  - hooks/HRangeRings2.cpp
  - section/RangeRings2.cpp

- Range ring performance improvement (don't render each ring twice)

  - hooks/RangeRings.cpp
  - section/RangeRings.cpp

- Camera performance improvements

  - hooks/CameraPerf.cpp

- Optimised some AI actions

  - hooks/aiinitattack.cpp

- Reduce overhead of population of Lua tables in C code. Affected functions:

  - UI
    - EntityCategoryFilterOut
    - EntityCategoryFilterDown
    - SelectUnits
  - Sim
    - EntityCategoryFilterDown

  * hooks/TableInsertFix.cpp

- Remove result table in `SelectUnits`

  - hooks/SelecUnitsTableFix.cpp

## Bugs

- Remove lingering transport load factor calcuation at aircraft initialization

  - hooks/RemoveTransportLoadFactor.cpp

- Fix `RolloverInfo` returning integer economy values.

  - hooks/GetRolloverInfoFix.cpp

- Fix [problem described here](https://www.lua.org/bugs.html#5.0.2-5)

  - hooks/weakgcmarkerFix.cpp

- Fix CUIWorldMesh:GetInterpolatedAlignedBox return table to properly store `ymax` and `zMax`

  - hooks/FixGetInterpolatedAlignedBox.cpp

- Make `TestCommandCaps` test commands caps instead of toggle caps

  - hooks/FixTestCommandCaps.cpp

- Reduce terrain collision distance-based offset to 1%;

- Prevent projectile layer changes on terrain collisions;

- Allows multiple collisions to be processed during collision checks.

  - hooks/FixCollisions.cpp

- Fix `CMauiControl:SetAlpha`: don't change color part and check for 3rd argument as boolean.

  - hooks/SetAlpha.cpp
  - section/SetAlpha.cpp

- Fix `UserDecal:SetPosition` argument.

  - hooks/DecalSetPositionFix.cpp

- Fix `ForkThread` function. Allow only lua function to be passed as first argument.

  - hooks/ForkThreadFix.cpp
  - section/ForkThreadFix.cpp

- Fix `Unit:SetStat` function, crashed before. Now returns true if value must be set.

  - hooks/SetStatFix.cpp
  - section/SetStatFix.cpp

- Upgrade Progress Fix

  - hooks/HUpgradeProgressFix.cpp
  - section/UpgradeProgressFix.cpp

- Prevents the crash with 'None' collisions of air. #3235

  - hooks/AirNoneCollisionFix.cpp

- Prevents commander exploding for no reason. #3406

  - hooks/NegativeIncomeFix.cpp

- Fix replays desyncing when a player leaves the game

  - hooks/DesyncFix.cpp
  - section/gpg_net.cpp
  - section/moho_decode.cpp
  - section/moho_input.cpp
  - section/include/desync_fix_global.h

- Kill exception during map loading

  - hooks/Kill_maploader_except.cpp

- Prevent blueprint editor being used without cheat mode

  - hooks/FixOpenBPEditor.cpp

- Can't become an observer while your human allies are still alive

  - hooks/ObserverFix.cpp

- Fix a crash when units arrive at waypoints
  - hooks/WayPointArrive.cpp

## Improvements

- Allows to use 4GB on x64

  - hooks/HFix4GB.cpp

- Make xact3d the error message print once

  - hooks/xact_3dapply.cpp
  - section/xact_3d_fix.cpp

- Improvements to lua messages

  - hooks/LuaMessages.cpp

- Adds the ability to bind the side mouse buttons (XButton1 and XButton2)
  - hooks/OnWindowMessage.cpp
  - section/OnWindowMessage.cpp

## Gameplay

- Change tick intel update interval from every 30 ticks to every 1 tick

  - hooks/IntelUpdate.cpp

- Removing CTRL formations

  - hooks/HRemovingCTRLFormations.cpp

- Ignore empty reclaim orders

  - hooks/HIgnoreEmptyReclaim.cpp

- Stops reclaim if unit is paused (similar to 'build', 'assist' etc.)

  - hooks/StopReclaimWhenPaused.cpp
  - section/StopReclaimWhenPaused.cpp
  - section/selectionPriority.cpp

- Allows changing army of ACUs

  - hooks/HTransferACUs.cpp

- Allow players to double-click to select Walls

  - hooks/WallSelection.cpp

- Make `LOWSELECTPRIO` apply to units under construction

  - hooks/selectionPriority.cpp

- Allow mass extractor template snapping

  - hooks/BuildTemplateSnap.cpp
  - section/BuildTemplateSnap.cpp

- Change how silos process their build progress (make it float instead of integer)
  - hooks/SiloProgress.cpp
  - section/SiloProgress.cpp

## Lua

- Change `SUBCOMMANDER` category name to `SACU_BEHAVIOR` (FAF makes this transparent)

  - hooks/CategoryRenames.cpp

- Stop the engine calling lua every time a plane turns
  - hooks/OnMotionTurnEvent.cpp

## Additions

These new features have been added in a backwards compatible manner

- Enable unused console commands: ren_Steering, dbg_Ballistics, dbg_EfxBeams, dbg_Trail, dbg_CollisionBeam, dbg_Projectile
  - hooks/EnableConsoleCommands.cpp
- Adds `GetHighlightCommand() - return table of command or nil` to UI (section/GetHighlightCommand.cpp)

- Adds new methods to the the `Projectile` class (section/ProjectileNewMethods.cpp):
  - `Projectile:SetNewTargetGroundXYZ(x, y, z)`
  - `x, y, z = Projectile:GetCurrentTargetPositionXYZ()`
  - `frequency = Projectile:GetZigZagFrequency()`
  - `maxzigzag = Projectile:GetMaxZigZag()`
- Adds Custom World Rendering (hooks: `HDraw.cpp`, `GetFPS.cpp`; section: `DrawFunc.cpp`, `DrawCircleSetColor.cpp`,`WorldView.cpp`)

  - `UI_DrawRect(pos:vector, size:float, color:string, thickness?=0.15:float)`
  - `UI_DrawCircle(pos:vector, radius:float, color:string, thickness?=0.15:float)`
  - `UI_DrawLine(pos1:vector, pos2:vector, color:string, thickness?=0.15:float)`

  These functions must be called within `WorldView:OnRenderWorld(delta_time)`. To enable CWR call `WorldView:SetCustomRender(true)` of WorldView you want to draw in. To disable call `WorldView:SetCustomRender(false)` respectively.

- Adds Strategic icon scale support:

  - hooks/IconScale.cpp
  - section/IconScale.cpp

- Adds new method to `WorldView` (section/WorldView.cpp):

  - `ProjectMultiple` projects multiple vectors at once

- Adds new methods for `UserUnit` (section/UserUnit.cpp):

  - `GetInterpolatedPosition` returns position of a unit interpolated by current frame
  - `GetFractionComplete` returns float from 0 to 1

- Adds method `MauiBitmap` (section/MauiBitmap.cpp):

  - `SetColorMask`- sets color mask for bitmap

- Adds 5th argument for `IssueMobileBuild` as flag whether to pick all units for build order

  - hooks/IssueMobileBuild.cpp
  - section/IssueMobileBuild.cpp

- Adds GetProcessAffinityMask, SetProcessAffinityMask, SetProcessPriority to "init.lua"

  - hooks/OnCreateInitLuaState.cpp
  - section/OnCreateInitLuaState.cpp

- Adds the commandType, position, targetId and blueprintId to sim unit:GetCommandQueue

  - hooks/SimGetCommandQueue.cpp
  - section/SimGetCommandQueue.cpp

- Adds new category 'CANLANDONWATER' for air units to be able to land on water

  - hooks/CanLandOnWater.cpp
  - section/Categories.cpp

- Adds new category 'OBSTRUCTSBUILDING' for props to block buildings from being build on top of those

  - hooks/Reclaimable.cpp
  - section/Categories.cpp

- Allows customize colors for team color mode

  - hooks/TeamColorMode.cpp
  - section/TeamColorMode.cpp

- Adds the order ID to the UserUnit:GetCommandQueue output

  - hooks/HOrderIDOutput.cpp
  - section/OrderIDOutput.cpp

- GetSessionClients also output maximum sim speed(maxSP)

  - hooks/HOutputMaxSP.cpp
  - section/OutputMaxSP.cpp

- Console command: "cam_DefaultMiniLOD 0" now disable mesh renderer for minimap

  - hooks/MinimapMesh.cpp
  - section/MinimapMesh.cpp

- Adds GetTimeForProfile to Sim and UI. Allows to deal with the loss of accuracy

  - section/GetTimeForProfile.cpp
  - section/LuaFuncRegs.cpp

- Adds optimized table.getsize2(~25 times faster) and table.empty2
  Adds getn2 as alias getn. Entry point to math functions queue

  - hooks/GetTableSize.cpp
  - section/LuaFuncRegs.cpp

- Maximum sim rate up to 50

  - hooks/DelClampMaxSimRate.cpp

- Adds GetDepositsAroundPoint to Sim and UI

  - section/SimGetDepositsAroundPoint.cpp
  - section/LuaFuncRegs.cpp

- Adds SetInvertMidMouseButton to UI

  - section/InvertMidMouseButton.cpp
  - section/LuaFuncRegs.cpp

- Unlock an "Ultra" graphics preset

  - hooks/UnlockUltraPreset.cpp

- Adds SetCommandSource to Sim. Allow armies to be shared by multiple players

  - section/SimSetCommandSource.cpp
  - section/LuaFuncRegs.cpp

- Adds GetMouseWorldPos to Sim

  - section/SimGetMouseWorldPos.cpp
  - section/LuaFuncRegs.cpp

- Adds SessionIsReplay to Sim

  - section/SimIsReplay.cpp
  - section/LuaFuncRegs.cpp

- Adds SetFocusArmy to Sim. Sets focus without restrictions

  - section/SimSetFocusArmy.cpp
  - section/LuaFuncRegs.cpp

- Allow upgrades to be queued on units under construction

  - hooks/BuildUnit.cpp
  - section/BuildUnit.cpp
  - section/SelectUnit.cpp
  - section/SimArmyCreate.cpp

- Add `Unit:ForceAltFootPrint` that forces game to use AltFootprint for the unit (applied to Salem in particular)
  - hooks/EntityGetFootprint.cpp
  - section/EntityGetFootprint.cpp
