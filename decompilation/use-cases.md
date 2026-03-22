# Potential Use Cases – FA-Binary-Patches

> [!IMPORTANT]
> **References vs Runtime Addresses**
> The game executable (`ForgedAlliance.exe`) statically links/inlines the engine DLLs. 
> The `MohoEngine.dll` addresses listed in these use cases are **for reference only** (useful when looking at the DLL in Ghidra to understand the logic). To actually implement these hooks in FAF, you must locate the inlined equivalents inside `ForgedAlliance.exe` by searching for matching assembly patterns or string references.

> Based on reverse-engineered engine internals. Each use case references the 
> specific functions/addresses needed and estimates implementation complexity.

---

## 1. Rejoin After Crash

**Goal**: Allow a crashed player to reconnect to a running game.

### Why It's Hard

The engine has a **built-in reconnect** mechanism (peer states 1-5 in `CLobby`), but it only handles **network drops**, not full client crashes. When a client crashes:
- Their `INetConnection` object is destroyed
- The host sees `OnConnectionLost` → sets state=6 (EJECTED) after timeout
- Once EJECTED, no recovery path exists

### Required Changes

| Step | Detail | Files |
|------|--------|-------|
| 1. Delay eject | Hook `OnConnectionLost` (DLL `0x1038DFA0`) – increase retry timeout or add new state 7 "WAITING_REJOIN" | MohoEngine.dll |
| 2. Save sim state | Call `Sim::GetBeatChecksum()` + serialize minimal sync state at eject point | MohoEngine.dll |
| 3. New client connects | Hook `OnConnectionMade` (DLL `0x1038DDA0`) – accept reconnect in state 7 | MohoEngine.dll |
| 4. Fast-forward | New client receives replay stream from beat 0 to current via `CReplayClient` | ForgedAlliance.exe |
| 5. Switch to live | Transition from replay mode (`isReplay=true`) to live mode – requires patching `VCR_SetupReplaySession` flow | MohoEngine.dll |
| 6. Checksum sync | Verify rejoin client matches via `Sim::VerifyChecksum` (DLL `0x103178A0`) | MohoEngine.dll |

### Key Functions

```
CLobby::OnConnectionLost    → DLL 0x1038DFA0  (hook: add state 7)
CLobby::OnConnectionMade    → DLL 0x1038DDA0  (hook: accept in state 7)
CLobby::EjectPeer           → DLL 0x1038F8F0  (prevent premature eject)
VCR_SetupReplaySession      → DLL 0x1043A940  (reuse for fast-forward)
CLIENT_CreateClientManager  → DLL 0x1012CD70  (create new client slot)
Sim::GetBeatChecksum        → DLL 0x103176D0  (verify sync)
g_SessionState              → DLL 0x109ba854  (session state control)
```

**Complexity**: 🔴 Very High – requires coordinating DLL hooks + state machine changes + replay-to-live transition.

---

## 2. Pause/Resume for Multiplayer

**Goal**: Reliable pause/resume in multiplayer games (currently buggy in FAF).

### Already Built-In

The engine already has full MP pause support:
- `CWldSession::RequestPause` (DLL `0x10456AB0`) – broadcasts to all peers
- `CWldSession::Resume` (DLL `0x10456B30`) – resume with validation
- `Sim::RequestPause` (DLL `0x10317A40`) – sim-side pause
- Pause state flows through `SSyncData[0x94]` → `CWldSession+0x464`

### Required Changes

| Step | Detail |
|------|--------|
| 1. Expose to Lua | Register `SessionRequestPause()` and `SessionResume()` as Lua callbacks |
| 2. UI integration | Call from game UI with proper validation |
| 3. Timeout | Add auto-resume after configurable timeout |

### Key Functions

```
CWldSession::RequestPause  → DLL 0x10456AB0
CWldSession::Resume        → DLL 0x10456B30
CWldSession::IsPaused      → DLL 0x10456BB0
CMarshaller::RequestPause  → DLL 0x102C1270
SSyncData[0x94]            → pauseState field
```

**Complexity**: 🟢 Low – the mechanism exists, just needs Lua bindings.

---

## 3. `SessionGetSimTickCount()` – Sim Tick Counter

**Goal**: Expose the current simulation tick to Lua for mods.

### Implementation

```cpp
// New Lua function: SessionGetSimTickCount()
// Read from Sim+0x910 (tickNumber, incremented in Sim::AdvanceBeat)
int Lua_SessionGetSimTickCount(lua_State* L) {
    Sim* sim = Sim::Get();  // DLL 0x101334B0
    lua_pushinteger(L, *(int*)((char*)sim + 0x910));
    return 1;
}
```

Already partially supported in existing patches (`SimSetCommandSource.cpp` pattern).

**Complexity**: 🟢 Very Low – single function registration.

---

## 4. `SessionGetSimChecksum()` – Checksum Access

**Goal**: Expose per-beat checksums to Lua for desync debugging tools.

### Implementation

```cpp
// New Lua function: SessionGetSimChecksum(beat)
int Lua_SessionGetSimChecksum(lua_State* L) {
    int beat = luaL_checkinteger(L, 1);
    Sim* sim = Sim::Get();
    MD5Digest hash;
    if (sim->GetBeatChecksum(beat, hash)) {  // DLL 0x103176D0
        char str[33];
        MD5Digest::ToString(hash, str);
        lua_pushstring(L, str);
    } else {
        lua_pushnil(L);
    }
    return 1;
}
```

**Complexity**: 🟢 Low – uses existing `GetBeatChecksum`.

---

## 5. `SessionGetClientList()` – Connected Clients

**Goal**: Expose connected client information to Lua.

### Implementation

```cpp
// Read from CClientManagerImpl+0x420 (mClients list)
int Lua_SessionGetClientList(lua_State* L) {
    CClientManagerImpl* mgr = GetClientManager();
    lua_newtable(L);
    int idx = 1;
    for (auto& client : mgr->mClients) {  // +0x420
        lua_newtable(L);
        lua_pushinteger(L, client->mIndex);       // +0x20
        lua_setfield(L, -2, "index");
        lua_pushinteger(L, client->mCommandSource); // +0x50
        lua_setfield(L, -2, "commandSource");
        lua_pushboolean(L, !client->mEjected);    // +0xC1
        lua_setfield(L, -2, "connected");
        lua_rawseti(L, -2, idx++);
    }
    return 1;
}
```

**Complexity**: 🟡 Medium – requires understanding the client list structure.

---

## 6. Mid-Game Replay Start

**Goal**: Start recording a replay at any point during a live game.

### Key Insight

`VCR_CreateReplay` (DLL `0x1043A090`) creates a replay **recording** stream. The `SSyncData` structure contains all per-beat data needed. In `DoBeat`, all entities, commands, and state are already serialized.

### Required Changes

| Step | Detail |
|------|--------|
| 1. Create replay file | Call `VCR_CreateReplay()` at any point |
| 2. Write header | Map name, army count, mods, random seed |
| 3. Hook DoBeat | After step 12 (pose updates), serialize SSyncData to replay file |

**Complexity**: 🟡 Medium – need to understand the exact replay serialization format.

---

## 7. Desync Debug Overlay

**Goal**: Show desync information in real-time during gameplay.

### Already Available

- `GPGNET_ReportDesync()` (DLL `0x10382800`) sends desync data to GpgNet
- `UI_ShowDesyncDialog()` (DLL `0x10403D30`) shows the desync popup
- `SDesyncInfo` at SSyncData[0x91] contains beat numbers + hash pairs
- `DoBeat` already calls `GPGNET_ReportDesync` for each desync info

### Enhancement

```cpp
// Hook DoBeat at the desync processing section
// Instead of just sending to GpgNet, also push to Lua:
LuaState* L = USER_GetLuaState();  // DLL 0x104876C0
LuaObject globals = L->GetGlobals();
LuaObject desyncTable;
desyncTable.AssignNewTable(L);
for (auto& info : desyncInfos) {
    desyncTable.SetInteger("beat", info.beat);
    desyncTable.SetString("localHash", info.localHash.ToString());
    desyncTable.SetString("remoteHash", info.remoteHash.ToString());
}
globals.SetObject("DesyncInfo", desyncTable);
```

**Complexity**: 🟢 Low – data is already processed, just needs Lua exposure.

---

## 8. Game Speed Control Modding

**Goal**: Allow finer-grained game speed control beyond "normal/fast/adjustable".

### Key Facts

- `CLIENT_CreateClientManager` takes `speedFlags`: 0=normal, 1=adjustable, 4=fast
- `CClientManagerImpl+0x450` = current game speed
- `CClientManagerImpl+0x454` = speed control enabled
- `CWldSession+0x458` = game speed (copied from SSyncData per beat)
- `WLD_SetGameSpeed` (DLL `0x10450720`) – existing API

### Possible Patch

Hook `LaunchGame` at the speed flag parsing (after `_stricmp` checks) to accept custom speed values from Lua options.

**Complexity**: 🟢 Low – single hook point in LaunchGame.

---

## 9. In-Game Save/Load State

**Goal**: Save and restore complete game state mid-game.

### Already Supported

- `ApplyPendingSaveData` (DLL `0x10457FA0`) is called in `DoBeat` if `CWldSession+0x4FC != 0`
- `GetSaveData` (DLL `0x10459660`) retrieves save data
- `CSaveGameRequestImpl` (EXE `0x00880F00`) handles save requests
- `SaveState` (DLL `0x10319E40`) serializes Sim state

### Enhancement: Quick-Save Patch

```cpp
// Hook: on key press, set CWldSession+0x4FC to trigger save
void QuickSave() {
    CWldSession* session = WLD_GetSession();  // DLL 0x104599F0
    // Write save data pointer at +0x4FC
    // Next DoBeat will call ApplyPendingSaveData()
}
```

**Complexity**: 🟡 Medium – the save infrastructure exists but may need careful state management.

---

## 10. Custom Network Protocol Messages

**Goal**: Add custom message types for mods (e.g., chat extensions, voting systems).

### Key Insight

`CClientBase::ProcessMessage` handles bytes 0x00-0xCB. Adding custom message types (0xD0+) is possible by hooking the message switch.

### Existing Example: Chat Message (0x37)

```
CClientBase::ProcessMessage → 0x37 → route via CClientManager+0x418
```

### Custom Messages

| Step | Detail |
|------|--------|
| 1. Define new msg type | e.g., 0xD0 = "Custom Lua Data" |
| 2. Hook ProcessMessage | Check for 0xD0, deserialize payload |
| 3. Send from Lua | New `SimCallback` that serializes via `ExecuteLuaInSim` (DLL `0x102C24F0`) |

**Complexity**: 🟡 Medium – hook ProcessMessage + define new message format.

---

## Summary: Effort vs. Impact Matrix

| Use Case | Complexity | Impact | Priority |
|----------|-----------|--------|----------|
| Sim Tick Counter | 🟢 Very Low | 🟡 Medium | P1 |
| Sim Checksum Access | 🟢 Low | 🟡 Medium | P1 |
| Pause/Resume MP | 🟢 Low | 🔴 High | P1 |
| Desync Debug Overlay | 🟢 Low | 🔴 High | P1 |
| Game Speed Control | 🟢 Low | 🟡 Medium | P2 |
| Client List | 🟡 Medium | 🟡 Medium | P2 |
| Mid-Game Replay | 🟡 Medium | 🟡 Medium | P3 |
| Quick-Save | 🟡 Medium | 🟡 Medium | P3 |
| Custom Net Messages | 🟡 Medium | 🔴 High | P2 |
| Rejoin After Crash | 🔴 Very High | 🔴 Very High | P4 |
