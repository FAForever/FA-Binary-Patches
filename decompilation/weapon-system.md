# Weapon & Damage System – Decompilation Notes

## 1. RUnitBlueprintWeapon Struct Layout
*Recovered from blueprint deserializer at `0x00522340`*

This struct stores the parsed blueprint data for a weapon before it is instantiated as a `CWeapon`.

| Offset | Type   | Name                           | Description / Lua equivalent |
|--------|--------|--------------------------------|------------------------------|
| `+0x08`| string | `Label`                        | Script ID for this weapon |
| `+0x24`| string | `DisplayName`                  | Display name |
| `+0x44`| bool   | `DummyWeapon`                  | No actual weapon created (e.g. Spiderbot feet) |
| `+0x45`| bool   | `PrefersPrimaryWeaponTarget`   | Targets what primary weapon targets |
| `+0x46`| bool   | `StopOnPrimaryWeaponBusy`  | Does not activate if primary has target |
| `+0x47`| bool   | `SlavedToBody`                 | Weapon slaved to unit body (must face to fire) |
| `+0x48`| float  | `SlavedToBodyArcRange`         | Arc range for slaved weapon |
| `+0x4C`| bool   | `AutoInitiateAttackCommand`    | Auto attack when idle |
| `+0x50`| float  | `TargetCheckInterval`          | Check interval for new target (default 3s) |
| `+0x54`| bool   | `AlwaysRecheckTarget`          | Always find better target |
| `+0x58`| float  | `MinRadius`                    | Minimum fire range |
| `+0x5C`| float  | `MaxRadius`                    | Maximum fire range |
| `+0x60`| float  | `MaximumBeamLength`            | specific beam weapon length |
| `+0x64`| float  | `EffectiveRadius`              | Effective range (offset 100) |
| `+0x68`| float  | `MaxHeightDiff`                | Max height range (weapons are cylinders) |
| `+0x6C`| float  | `TrackingRadius`               | Multiplier of MaxRadius for tracking |
| `+0x70`| float  | `HeadingArcCenter`             | Firing arc center (degrees) |
| `+0x74`| float  | `HeadingArcRange`              | Firing arc tolerance |
| `+0x78`| float  | `FiringTolerance`              | Aim accuracy required to take shot |
| `+0x7C`| float  | `FiringRandomness`             | Gaussian random aim offset |
| `+0x80`| float  | `RequiresEnergy`               | Energy per shot |
| `+0x84`| float  | `RequiresMass`                 | Mass per shot |
| `+0x88`| float  | `MuzzleVelocity`               | Projectile speed |
| `+0x8C`| float  | `MuzzleVelocityRandom`         | Random variation |
| `+0x90`| float  | `MuzzleVelocityReduceDistance` | Distance to reduce velocity for higher arc |
| `+0x94`| bool   | `LeadTarget`                   | True to lead target when aiming |
| `+0x98`| float  | `ProjectileLifetime`           | 0 = use projectile BP lifetime |
| `+0x9C`| float  | `ProjectileLifetimeUsesMultiplier` | Mult * (MaxRadius/MuzzleVelocity) |
| `+0xA0`| float  | `Damage`                       | **How much damage to cause** |
| `+0xA4`| float  | `DamageRadius`                 | **Radius of splash damage** |
| `+0xA8`| string | `DamageType`                   | E.g. "Normal", "Overcharge" |
| `+0xC4`| float  | `RateOfFire`                   | **Shots per second** |
| `+0xC8`| string | `ProjectileId`                 | Projectile blueprint ID (offset 200) |
| `+0xE8`| string | `TargetRestrictOnlyAllow`      | Comma separated allowed categories |
| `+0x104`| string | `TargetRestrictDisallow`       | Comma separated disallowed categories |
| `+0x120`| bool   | `ManualFire`                   | Never fires automatically |
| `+0x121`| bool   | `NukeWeapon`                   | Flag |
| `+0x122`| bool   | `OverChargeWeapon`             | Flag |
| `+0x123`| bool   | `NeedPrep`                     | Requires prep time |
| `+0x124`| bool   | `CountedProjectile`            | Built/stored before firing |
| `+0x128`| int    | `MaxProjectileStorage`         | Silo capacity |
| `+0x134`| int    | `AttackGroundTries`            | Shots at ground before moving on |
| `+0x138`| bool   | `AimsStraightOnDisable`        | Reset aim |
| `+0x139`| bool   | `Turreted`                     | On turret |
| `+0x13A`| bool   | `YawOnlyOnTarget`              | Ignore pitch for considering "on target" |
| `+0x149`| bool   | `IgnoreIfDisabled`             | Don't fire if disabled |
| `+0x14A`| bool   | `CannotAttackGround`           | Cannot target ground |

## 2. Damage Application Pipeline

The Lua scripts define `OnDamage` handlers on units and projectiles.
`Entity::OnDamage` (or `Unit::OnDamage`) is invoked by the engine through C++-to-Lua wrappers.

Decompiled wrappers:
### `Entity::OnDamage` (Lua wrapper)
- `0x0073a4f0` -> prepares Lua stack, pushes entity and parameters, string `"OnDamage"`, calls `lua_pcall` via `FUN_0073ac30`.
- `0x005ec040` -> simpler version doing the same via `FUN_005ed960`.

### The Core Damage Application Logic (`0x005E73E0`)
The actual C++ loop that applies damage to targets resides at `0x005E73E0`. 
This function takes the Weapon/Projectile object, computes damage, and calls the `OnDamage` handlers.
1. It queries the `"Damage"` field from the entity's blueprint using a Lua getter loop.
2. It calls the Lua method `"CheckCanTakeDamage"` (string at `0x00e1ef90`).
3. If true, it calls the `OnDamage` wrapper (`0x005ec040`).
4. `param_1` (the weapon/projectile) stores its owning `CUnit*` at offset `+0x0C` (passed to `OnDamage`).

### `Projectile:SetDamage(amount, radius)`
- Registered via LuaFuncDef at `0x006a1d50`.
- Wrapper label is `LAB_006a1d30`.

### `Entity:Kill(instigator, type, excessDamageRatio)`
- Registered via LuaFuncDef at `0x00691cb0`.
- Wrapper label is `LAB_00691c90`.

## 3. Weapon Array / `IAttacker` Interface

The unit's weapons are stored in a standard `std::vector<CWeapon*>` array.
From decompiling `AttackerGetWeaponByIndex` (`0x005D77D0`):

```cpp
CWeapon* __thiscall AttackerGetWeaponByIndex(IAttacker* this, uint index) {
    CWeapon** start = *(CWeapon***)(this + 0x5C);
    CWeapon** end   = *(CWeapon***)(this + 0x60);
    
    if (start != nullptr && index < (end - start)) {
        return start[index];
    }
    // "Invalid weapon index %i passed to AttackerGetWeaponByIndex."
}
```

The `IAttacker` interface (or base class of `CUnit`) holds:
- `+0x5C`: `std::vector<CWeapon*>` `_M_start`
- `+0x60`: `std::vector<CWeapon*>` `_M_finish`
- `+0x64`: `std::vector<CWeapon*>` `_M_end_of_storage`

*Note: FAF's CUnit inherits from multiple interfaces. The `IAttacker` fields are at some offset within `CUnit`, likely near the beginning or immediately after a vtable.*

## 4. Real CWeapon Class

The string `"CWeaponAttributes"` (RTTI `.AVCWeaponAttributes@Moho@@` at `0x00E2D8C0`) corresponds to the runtime instantiated weapon object properties. The `RUnitBlueprintWeapon` is loaded at load-time; `CWeapon` handles the actual simulation state (current target, cooldown timers, pitch/yaw state).

