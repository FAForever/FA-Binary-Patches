# FA Engine Patches Repository

This are some binary patches for Supreme Commander Forged Alliance.

To apply them build or get the patcher itself from here

https://github.com/FAForever/FA_Patcher

This are just the patch files for this game. I decided to separate them from patcher source code.

# Change List
## Fixes
- Fix `ForkThread` function. Allow only lua function to be passed as first argument.
    - hooks/ForkThreadFix.cpp
    - section/ForkThreadFix.cpp
- Fix `Unit:SetStat` function, crashed before. Now returns true if value must be set.
    - hooks/SetStatFix.cpp
    - section/SetStatFix.cpp
- Upgrade Progress Fix
    - hooks/HUpgradeProgressFix.cpp
    - section/UpgradeProgressFix.cpp
- Allows to use 4GB on x64
    - hooks/HFix4GB.cpp
- Reduce call sim beat of UI
    - hooks/HUIReduceCallSimBeat.cpp
- Removing CTRL formations
    - hooks/HRemovingCTRLFormations.cpp
- Ignore empty reclaim orders
    - hooks/HIgnoreEmptyReclaim.cpp
- Zeroing GuardScanRadius if the unit's order is not Patrol/AttackMove. Increases performance.
    - hooks/HGuardFix.cpp
    - section/GuardFix.cpp
- Prevents the crash with 'None' collisions of air. #3235
    - hooks/AirNoneCollisionFix.cpp
- Prevents commander exploding for no reason. #3406
    - hooks/NegativeIncomeFix.cpp
- Range ring performance improvement (reducing height cylinders)
    - hooks/HRangeRings2.cpp
    - section/RangeRings2.cpp
- Range ring performance improvement (don't render each ring twice)
    - hooks/RangeRings.cpp
    - section/RangeRings.cpp
- Camera performance improvements
    - hooks/CameraPerf.cpp
- Fix replays desyncing when a player leaves the game
    - hooks/DesyncFix.cpp
    - section/gpg_net.cpp
    - section/moho_decode.cpp
    - section/moho_input.cpp
    - section/include/desync_fix_global.h
- Kill exception during map loading
    - hooks/Kill_maploader_except.cpp
- Make xact3d the error message print once
    - hooks/xact_3dapply.cpp
    - section/xact_3d_fix.cpp
- Prevent blueprint editor being used without cheat mode
    - hooks/FixOpenBPEditor.cpp
- Can't become an observer while your human allies are still alive
    - hooks/ObserverFix.cpp
- Stop the engine calling lua every time a plane turns
    - hooks/OnMotionTurnEvent.cpp
- Optimised some AI actions
    - hooks/aiinitattack.cpp
- Fix a crash when units arrive at waypoints
    - hooks/WayPointArrive.cpp
- Improvements to lua messages
    - hooks/LuaMessages.cpp

## Additions
- Adds the commandType, position, targetId and blueprintId to sim unit:GetCommandQueue
    - hooks/SimGetCommandQueue.cpp
    - section/SimGetCommandQueue.cpp
- Adds new category 'CANLANDONWATER' for air units to be able to land on water
    - hooks/CanLandOnWater.cpp
    - section/Categories.cpp
- Adds new category 'OBSTRUCTSBUILDING' for props to block buildings from being build on top of those
    - hooks/Reclaimable.cpp
    - section/Categories.cpp
- Allows changing army of ACUs
    - hooks/HTransferACUs.cpp
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
- Allow players to double-click to select Walls
    - hooks/WallSelection.cpp
- Adds GetTimeForProfile to Sim and UI. Allows to deal with the loss of accuracy
    - section/GetTimeForProfile.cpp
    - section/LuaFuncRegs.cpp
- Adds optimized table.getsize2(~25 times faster) and table.empty2
  Adds getn2 as alias getn. Entry point to math functions queue
    - hooks/GetTableSize.cpp
    - section/LuaFuncRegs.cpp
- Maximum sim rate up to 50
    - hooks/DelClampMaxSimRate.cpp
- Adds an amphibious mode toggle to the Salem
    - hooks/luaSimGetStat.cpp
    - section/luaSimGetStat.cpp
- Make `LOWSELECTPRIO` apply to units under construction
    - hooks/selectionPriority.cpp
    - section/selectionPriority.cpp
- Adds GetDepositsAroundPoint to Sim and UI & Entry point to ui functions queue
  Allow the getting of Mass or Energy spots around a location
    - hooks/UserGetDepositsAroundPoint.cpp
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
    - section/LuaFuncRegs.cpp
- Adds SessionIsReplay to Sim
    - section/SimIsReplay.cpp
    - section/LuaFuncRegs.cpp
- Adds SetFocusArmy to Sim. Sets focus without restrictions
  Entry point to sim functions queue
    - hooks/SimSetFocusArmy.cpp
    - section/SimSetFocusArmy.cpp
    - section/LuaFuncRegs.cpp
- Allow upgrades to be queued on units under construction
    - hooks/BuildUnit.cpp
    - section/BuildUnit.cpp
    - section/SelectUnit.cpp
    - section/SimArmyCreate.cpp