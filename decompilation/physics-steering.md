# Physics & Steering System – Decompilation Notes

*(ForgedAlliance.exe v3599 @ 0x00400000)*

## 1. Physics Blueprint Struct (`RUnitBlueprintPhysics`)
Extracted from the blueprint deserializer at `0x00520A40`. This data dict defines the unit's max constraints.

| Offset  | Type   | Name                    | Description                     |
|---------|--------|-------------------------|---------------------------------|
| `+0x18` | enum   | `MotionType`            | Method of locomotion            |
| `+0x1C` | enum   | `AltMotionType`         | Alternate locomotion            |
| `+0x24` | float  | `DiveSurfaceSpeed`      | Submarine dive/surface speed    |
| `+0x28` | float  | `MaxSpeed`              | Peak forward velocity           |
| `+0x2C` | float  | `MaxSpeedReverse`       | Peak reverse velocity           |
| `+0x30` | float  | `MaxAcceleration`       | Max acc magnitude               |
| `+0x34` | float  | `MaxBrake`              | Max braking deceleration        |
| `+0x38` | float  | `MaxSteerForce`         | Max steering force magnitude    |
| `+0x3C` | float  | `BankingSlope`          | Corner banking angle            |
| `+0x40` | float  | `RollStability`         | Roll resistance (0 to 1)        |
| `+0x44` | float  | `RollDamping`           | Damp against rolling motion     |
| `+0x48` | float  | `WobbleFactor`          | Hover wobble amplitude          |
| `+0x4C` | float  | `WobbleSpeed`           | Hover wobble frequency          |
| `+0x50` | float  | `TurnRadius`            | Physical turning radius         |
| `+0x54` | float  | `TurnRate`              | Degrees per second              |
| `+0x58` | float  | `TurnFacingRate`        | Hover unit facing damping       |
| `+0x5C` | bool   | `RotateOnSpot`          | If unit can rotate without moving |
| `+0x60` | float  | `RotateOnSpotThreshold` | When in-motion rotate takes effect |

*These constants are essential to properly throttle/clamp our custom steering vectors in the proposed hook!*

---

## 2. Steering Tick (`CAiSteeringImpl`)
The core steering forces (Seek/Separation) are calculated inside `CAiSteeringImpl`.

Main Update Loop: **`0x005d32b0`**
- Prints `"0x%08x's steering tick.\n"` with the Unit ID.
- Resolves its owner `CUnit*` via `this + 0x1C`.
- Depending on the `MotionType` or state (`this+0x58`), routes to one of the force summing functions:
  1. `FUN_005d3140()`
  2. `DriveToNextWaypoint()` (`FUN_005d3000` / `0x005d2c00`)

### The Custom Hook Objective
To fix units pushing/bumping into each other (improving Separation), we must place a `Detour` hook around `FUN_005d32b0` (the main tick) or `FUN_005d2c00` (ProcessSplineMovement / path movement).
Inside our Detour, we will:
1. Re-calculate positions of all nearby units.
2. Apply an RVO (Reciprocal Velocity Obstacle) or enhanced flocking separation vector.
3. Clamp the resulting vector using the blueprint's `MaxSteerForce` (`+0x38`) and `MaxAcceleration` (`+0x30`).
4. Inject this modified velocity vector back into the engine physics solver.

---

## 3. CAiSteeringImpl – Confirmed Structure (v22)

### Hook Site (Verified via Ghidra)
| Address      | Bytes       | Instruction                  |
|--------------|-------------|------------------------------|
| `0x005D32B0` | `55`        | PUSH EBP (prologue start)    |
| `0x005D32B1` | `8B EC`     | MOV EBP, ESP                 |
| `0x005D32B3` | `83 E4 F8`  | AND ESP, 0xFFFFFFF8          |
| `0x005D32B6` | `83 EC 58`  | SUB ESP, 0x58                |
| `0x005D32B9` | `53`        | PUSH EBX                     |
| `0x005D32BA` | `55`        | PUSH EBP                     |
| `0x005D32BB` | `8B D9`     | MOV EBX, ECX ← **HOOK SITE**|
| `0x005D32BD` | `8B 43 1C`  | MOV EAX, [EBX+0x1C]         |
| `0x005D32C0` | `56`        | PUSH ESI                     |
| `0x005D32C1` |             | ← **RETURN ADDRESS**         |

### Confirmed Offsets
| Offset  | Type     | Name    | Notes                          |
|---------|----------|---------|--------------------------------|
| `+0x1C` | CUnit*   | owner   | Direct unit pointer (VERIFIED) |
| `+0x58` | int      | state   | 0x10 or 0x20 = active states  |

### IMPORTANT: Correction to Earlier Documentation
The path "CAiSteeringImpl+0x1C → CAiNavigatorImpl → +0x5C → CUnit*"
is WRONG. The correct path is:
  CAiSteeringImpl+0x1C = CUnit* directly (no intermediate hop)

The navigator back-pointer at CAiNavigatorImpl+0x5C exists but is
NOT the path from steering to unit.

### Calling Convention
- __thiscall: ECX = CAiSteeringImpl*
- EBX = ECX (set at hook site)
- EAX = [EBX+0x1C] = CUnit* (set at hook site)
- Trampoline must restore: MOV EBX,ECX / MOV EAX,[EBX+0x1C] / PUSH ESI
- Return to: 0x005D32C1
