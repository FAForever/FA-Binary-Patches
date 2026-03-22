# Pathfinding & AI Platoon System – Decompilation Notes

## 1. Unit Navigation (`CAiNavigatorImpl`)

Every mobile unit manages its movement via a `CAiNavigatorImpl` instance.
The navigator acts as the interface between the core pathfinder and the unit task queue.

### Structural Offsets:
- **In `CUnit`:**
  - `+0x4B4`: Pointer to the active `CAiNavigatorImpl*`.
- **In `CAiNavigatorImpl`:**
  - `+0x5C` (`this[0x17]`): Back-pointer to the owning `CUnit*`.
  - Used internally for debug logging (`mUnit+0x70` identity print) and state management.

### Lua Interface (C++ to Lua Wrappers):
The navigator exposes a comprehensive suite of functions to Lua scripts (State Machine / Task Scripts):
- `CanPathToGoal`
- `AtGoal`
- `GetGoalPos`
- `GetCurrentTargetPos`
- `GetStatus`
- `HasGoodPath`
- `SetSpeedThroughGoal`
- `AbortMove`
- `SetDestUnit`
- `SetGoal`

Implementation example: `CAiNavigatorImpl::AbortMove()` was found at `0x005a3750`.

## 2. Core Pathfinder (`CAiPathFinder`)

The actual A* calculations are handled by `CAiPathFinder`.
- **RTTI Name:** `.?AVCAiPathFinder@Moho@@` (`0x00F6BABC`).
- **Configuration:** It operates asynchronously over multiple ticks to prevent simulation stuttering. Key configuration strings found in the binary:
  - `"Maximum number of steps to run pathfinder in background"` (`0x00E33758`)
  - `"Maximum number of ticks to allow pathfinder preview to take"` (`0x00E35C30`)

## 3. Platoon Management (`CPlatoon` & `CAiBrain`)

The game's AI does not command single units individually; it commands **Platoons**.
Each AI controls an army via a `CAiBrain` instance, which allocates units into `CPlatoon` objects.

### `CPlatoon` Features:
- **Class:** `CPlatoon`
- **Methods:**
  - `GetBrain()` – Returns the owning `CAiBrain`.
  - `IsMoving()`, `IsAttacking()`, `IsPatrolling()`, `IsFerrying()` – State getters.
  - `GetSquadUnits()`, `GetSquadPosition()` – Formation management.
  - `FormPlatoon()`, `CanFormPlatoon()`.

### `CAiBrain` Integration:
The `CAiBrain` handles the creation and assignment of units to platoons.
Lua wrappers include:
- `MakePlatoon()`
- `BuildPlatoon()`
- `AssignUnitsToPlatoon()`
- `GetPlatoonsList()`
- `DisbandPlatoon()`
- `GetNumPlatoonsWithAI()`

### Execution Path:
`CAiBrain` generates `CPlatoon` objects $\rightarrow$ Lua assigns AI plans to the platoon $\rightarrow$ Platoon issues commands to `CUnit` $\rightarrow$ `CUnitMoveTask` uses `CAiNavigatorImpl` $\rightarrow$ `CAiPathFinder` computes the route.
