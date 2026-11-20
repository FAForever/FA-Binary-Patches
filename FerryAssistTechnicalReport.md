# Ferry Assist Engine Patch Report

## Executive summary

The breakthrough was discovering that a ferry route is not fundamentally identified by its visible waypoints or by the beacon's entity ID. It is represented by one or more native `CUnitCommand` objects. Those command objects contain the hidden relationship to the ferry beacon and can belong to multiple transports simultaneously.

The successful patch therefore does not recreate the route. It takes the exact existing ferry command objects resolved by `CUnitFerryTask` and adds the assisting transport to those commands through `CUnitCommandQueue::InsertCommandToQueue`.

```text
Original transport queue --+
                           +-- same CUnitCommand objects -- ferry beacon
Assistant transport queue -+
```

This is quite different from copying waypoint coordinates. Reissuing `IssueFerry` creates new command objects and therefore normally creates a new beacon.

The clean-room recompile was extremely useful because it exposed:

- The ferry task's fields and state machine.
- How a guard order on a ferry beacon becomes a `CUnitFerryTask`.
- How the task locates an existing route owner.
- How command queues store weak references to shared command objects.
- The correct command-queue insertion function and its unusual calling convention.

The final PR implementation is in:

- `section/FerryAssist.cpp`
- `hooks/FerryAssist.hook`
- The Ferry Assist entry in `changelog.md`

## 1. Relevant information in the clean-room recompile

The main upstream reference was [Draiget/faf-re](https://github.com/Draiget/faf-re). It is a reconstruction/disassembly project for the Forged Alliance engine rather than original game source. Its README warns that recovered source edges still require verification against the binary.

### 1.1 `CUnitGuardTask.cpp`

Relevant file:

[CUnitGuardTask.cpp](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/tasks/CUnitGuardTask.cpp)

This explains how ordinary transport assistance reaches the ferry subsystem.

When a transport guards another transport or a `FERRYBEACON`, the guard task sets a ferry-redirection flag. In the guard task's processing state, if the active guarded unit is a beacon, it calls:

```cpp
CUnitFerryTask::Create(mCommandTask, mSecondaryUnit.GetObjectPtr());
```

The corresponding executable call sites are:

```text
0x0061465B
0x0061466F
```

This was the key to instant promotion. Hooking later ferry processing works, but only after the transport has begun the normal ferry-assistance cycle. Hooking the task creation calls allows promotion immediately after the ferry task has been initialized.

### 1.2 `CUnitFerryTask.h`

Relevant file:

[CUnitFerryTask.h](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/tasks/CUnitFerryTask.h)

This provided the verified task layout:

| Offset | Recovered field | Patch use |
|---:|---|---|
| `0x1C` | inherited owner transport reference | Assisting transport |
| `0x38` | `mFollowsExistingRoute` | Confirms this is route assistance |
| `0x48` | `mRouteUnit` | Original target of the guard/ferry task |
| `0x50` | `mFerryUnit` | Factory or transport route source |
| `0x58` | `mBeacon` | Resolved ferry beacon |

The patch deliberately maps only the fields it needs and uses `static_assert` for the verified offsets.

### 1.3 `CUnitFerryTask.cpp`

Relevant file:

[CUnitFerryTask.cpp](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/tasks/CUnitFerryTask.cpp)

This was the most important source.

#### Beacon-targeted constructor

The constructor at `0x0060DFC0` receives the parent command task and target unit.

When the target has the `FERRYBEACON` category, it:

- Marks the task as following an existing route.
- Stores the beacon.
- Uses the beacon position as the ferry position.

That confirmed that a guard command directed at a ferry beacon already enters a dedicated engine path. The patch did not need to invent a new method of detecting ferry assistance.

#### `GetUnitCommands`

The function at `0x0060EB50` resolves the route commands for the ferry task.

For a beacon-targeted task, it:

1. Enumerates the transport's army's `TRANSPORTATION` units.
2. Rejects dead, destruction-queued, and unfinished transports.
3. Requires the candidate to report the Ferrying state.
4. Reads its current command.
5. Calls `CUnitCommand::GetFerryBeacon()`.
6. Compares that result with the target beacon.
7. Copies the candidate's command-vector weak references into the temporary result vector.

This was almost exactly the route-owner search previously attempted in Lua. The major advantage is that the engine already knows all its own eligibility rules and returns the exact native route objects.

The patch reuses this function instead of independently reconstructing the search.

#### Three route-resolution call sites

The ferry task calls `GetUnitCommands` in three phases:

```text
0x0060ED96
0x0060EEF1
0x0060F0E9
```

These correspond to route movement and unloading behavior. They became fallback hook locations. If the route is not resolvable when the ferry task is first constructed, a later state-machine pass can still promote the assistant.

### 1.4 `CUnitCommandQueue.h/.cpp`

Relevant files:

- [CUnitCommandQueue.h](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/CUnitCommandQueue.h)
- [CUnitCommandQueue.cpp](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/CUnitCommandQueue.cpp)

These established the queue layout and mutation API:

```text
CUnitCommandQueue::mUnit        +0x08
CUnitCommandQueue::mCommandVec  +0x0C
```

The important functions were:

```text
0x006EDCB0  CUnitCommandQueue::InsertCommandToQueue
0x006EDE70  CUnitCommandQueue::RemoveFirstCommandFromQueue
```

`InsertCommandToQueue` does more than insert a pointer into a vector. It:

- Adds the unit to the command's unit set.
- Inserts a weak reference into that unit's queue.
- Updates the simulation digest with the unit ID and command ID.
- Marks the queue for UI refresh.
- Emits the command-inserted event.

That is why calling this function is safer than manually editing the vector.

### 1.5 `CUnitCommand.cpp`

Relevant file:

[CUnitCommand.cpp](https://github.com/Draiget/faf-re/blob/master/src/sdk/moho/unit/CUnitCommand.cpp)

`CUnitCommand::AddUnit` at `0x006E8B40` demonstrated the central ownership model:

```cpp
mUnitSet.Add(unit);
queue.insert(queue.begin() + index, WeakPtr<CUnitCommand>(this));
```

A command is not owned exclusively by one unit. It maintains a set of participating units, while every participating unit has a weak reference to that same command in its queue.

Therefore, the order system is not simply:

```text
Unit -> private array of copied order records
```

It is closer to:

```text
CUnitCommand
  +-- unit set: transport A, transport B, ...
  +-- target
  +-- command type
  +-- route/beacon state
  +-- other shared command data

Unit A command queue -> weak reference to command
Unit B command queue -> weak reference to same command
```

That is the mechanism the ferry patch uses.

## 2. How transports gain ownership

### Step 1: Let vanilla construct the ferry task

The patch does not replace guard-command dispatch.

The assisting transport receives an ordinary guard order on the ferry beacon. Vanilla creates a fully initialized `CUnitFerryTask`. This preserves normal validation and target resolution.

### Step 2: Resolve the existing route

Immediately after `CUnitFerryTask::Create` returns, the wrapper calls the original engine function:

```cpp
CUnitFerryTask::GetUnitCommands(routeCommands);
```

For a beacon target, that function searches for an existing transport whose current command resolves to the same beacon.

### Step 3: Verify the operation is narrowly applicable

Promotion requires:

- A valid ferry task.
- `mFollowsExistingRoute` set.
- A valid beacon.
- `mRouteUnit` resolving to that beacon.
- A valid assisting transport and command queue.
- Exactly one command in the assistant's queue.
- That command being type `15`, Guard.
- A nonempty resolved route.
- Every resolved route command being type `17`, Ferry.

The exact-one-guard requirement prevents the patch from modifying arbitrary queues or shifted command sequences.

### Step 4: Insert the exact ferry command objects

The patch calls:

```cpp
InsertCommandToQueue(assistantQueue, existingFerryCommand, index);
```

It does not construct replacements.

This causes `CUnitCommand::AddUnit` to:

- Add the assistant to the existing command's unit set.
- Add a weak reference to the same command to the assistant's queue.

Because the command object already contains the beacon association, the assistant immediately reports the original beacon through `GetTransportFerryBeacon()`.

### Step 5: Remove the guard order

The new ferry commands are first inserted after the guard:

```text
Before:
[Guard beacon]

Temporary:
[Guard beacon, Ferry command 1, Ferry command 2, ...]

After:
[Ferry command 1, Ferry command 2, ...]
```

Only after all ferry commands are present does the patch call:

```cpp
RemoveFirstCommandFromQueue(assistantQueue);
```

This ordering is deliberate. It prevents the task from observing an empty command queue while it is executing.

### Step 6: Retain fallback hooks

The immediate constructor hooks solve the delayed-assistance behavior. The three original `GetUnitCommands` hooks remain as fallbacks for cases where an existing owner or complete route cannot yet be resolved at task construction time.

## 3. Why this creates shared ownership

A ferry command object carries the state from which `GetFerryBeacon()` resolves the beacon.

If a second transport is attached to the same command object, both transports' queues refer to the same route identity:

```text
Transport A queue[0] == command pointer X
Transport B queue[0] == command pointer X
command X -> original beacon
```

This explains the successful test invariants:

```text
assistant:GetTransportFerryBeacon() == originalBeacon
assistant queue head type == 17
assistant command pointer == owner command pointer
```

It also explains why route editing and owner removal worked in the native tests: the promoted transports are not approximating the route. They participate in the engine's shared command ownership model.

## 4. Approaches that failed

### 4.1 Reissuing `IssueFerry`

The first Lua approach copied the owner's waypoint positions and ran:

```lua
IssueClearCommands({ assistant })

for _, position in route do
    IssueFerry({ assistant }, position)
end
```

This reproduced the visible route but created new command objects. Each assistant consequently created its own beacon.

Proximity to the original beacon did not cause route merging.

Lesson:

> Route coordinates are route geometry, not route identity.

### 4.2 Waiting until the assistant reached the beacon

Waiting avoided creating a beacon at the assistant's starting location, but it did not change `IssueFerry` semantics. The assistant still created a separate route.

It also caused an undesirable vanilla cycle:

1. Assistant guarded the beacon.
2. It began picking up units.
3. It returned to the pickup point.
4. Only then did the later native hook promote it.

This is why the task-construction hook was added for instant promotion.

### 4.3 Persistent helper transport

A helper unit was tested as a permanent route owner. It required increasingly invasive transport characteristics:

- `TRANSPORTATION` category.
- An attachment bone.
- Cargo capacity information.
- `RULEUCC_Transport`.
- Actual compatibility with engine transport internals.

Even when made cargo-like, it did not solve the underlying ownership problem cleanly. It was a workaround for route lifetime rather than participation in the real command model.

Lesson:

> Preserve ownership through the engine's command ownership system, not through a synthetic immortal unit.

### 4.4 Copying queue-visible Lua data

Lua exposed:

- Command type.
- Target ID.
- Position.
- Beacon getter results.

That was enough to diagnose behavior, but not enough to reproduce the hidden native associations. Two commands can look identical in Lua while referring to different native ferry beacons.

### 4.5 Incorrect insertion calling convention

The first native insertion attempt crashed because `InsertCommandToQueue` is not an ordinary `thiscall` in the relevant binary.

The working convention is:

```text
EAX   = CUnitCommand*
EBX   = CUnitCommandQueue*
stack = insertion index
```

The final stub is therefore:

```cpp
asm volatile(
    "push %[index];"
    "call 0x006EDCB0;"
    : "+a" (command)
    : "b" (queue),
      [index] "r" (index)
    : "ecx", "edx", "memory"
);
```

The `"+a"` constraint matters because the call may overwrite `EAX`.

Lesson:

> A correct function address is insufficient; register and stack contracts must also be correct.

### 4.6 Misreading weak-reference entries

The queue does not store a direct `CUnitCommand*`. Its vector contains a weak-object link pointing four bytes into the command object.

The patch must decode it:

```cpp
command = entry.CUnitCommand - 4;
```

Treating the stored value as the object base produces incorrect field reads and can crash when passed back into engine functions.

### 4.7 Emptying the queue too early

Removing the guard first would temporarily leave the active ferry/guard task with no current command. That can trigger task teardown, queue-finished behavior, or state transitions before promotion is complete.

The safe sequence was:

1. Insert shared ferry commands after Guard.
2. Remove Guard last.

### 4.8 Temporary route-vector lifetime

`GetUnitCommands` constructs a temporary vector of intrusive weak references. In the immediate constructor wrapper, that temporary must be properly destroyed:

1. Unlink each weak reference from its owner chain.
2. Release the vector storage with the engine allocator.
3. Clear the vector pointers.

Using the ordinary C++ runtime allocator or dropping the vector without unlinking could leak, corrupt the weak-link chains, or cross allocator boundaries.

## 5. Multiplayer and replay implications

The queue mutation occurs inside the deterministic simulation and `InsertCommandToQueue` updates the simulation context using the unit ID and command ID.

That is desirable when every peer runs the patch: each peer receives the same guard order, resolves the same route, and performs the same queue mutation.

It is not mixed-peer compatible. Testing with patched and unpatched peers desynchronized immediately because:

- Patched peer: assistant's Guard becomes shared Ferry commands.
- Unpatched peer: assistant retains vanilla Guard/ferry-assist behavior.

The command stream may be identical, but the resulting simulation state differs.

Replay and save/load still deserve dedicated testing:

- Replays should work when replayed with a compatible executable because the deterministic guard input should reproduce promotion.
- An unpatched executable replaying a patched match should diverge.
- Save/load exercises serialization of tasks, command unit sets, queues, and weak references.
- Ferry UI markers already have vanilla save/load limitations, so marker loss alone would not prove simulation corruption.

## 6. Lessons for other shared-order features

The patch demonstrates a broader engine capability:

> To make another unit participate in an existing order, add that unit to the existing `CUnitCommand` and insert a weak reference to that command in the unit's queue.

This can potentially apply to:

- Engineer build queues.
- Patrol chains.
- Attack sequences.
- Reclaim queues.
- Repair or capture orders.
- Coordinated movement.
- Factory-related command ownership, although factory queues are structurally different.

The safest implementation pattern is:

1. Identify a precise user or task transition.
2. Resolve the source unit and destination unit deterministically.
3. Obtain the existing `CUnitCommand*` objects.
4. Validate every command type.
5. Verify the destination can execute those commands.
6. Insert through `CUnitCommandQueue::InsertCommandToQueue`.
7. Remove the triggering command only after replacement commands exist.
8. Let engine APIs manage command unit sets, weak references, events, and sync hashing.

## Conclusion

The key finding was that ferry ownership belongs to native command objects, not merely to transports, beacons, or waypoint coordinates.

The clean-room recompile revealed the complete path:

```text
Guard beacon
  -> CUnitGuardTask
  -> CUnitFerryTask::Create
  -> CUnitFerryTask::GetUnitCommands
  -> existing route CUnitCommand objects
  -> CUnitCommandQueue::InsertCommandToQueue
  -> assistant added to command unit sets
  -> shared route ownership
```

The general lesson is powerful: many FA orders are multi-unit command objects referenced by per-unit queues. Adding a unit to an existing order is therefore possible, but it must go through the engine's command membership and queue APIs. Copying visible order data is not equivalent, and directly editing queue storage bypasses essential lifetime, event, and synchronization behavior.
