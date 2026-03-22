# ForgedAlliance.exe – Reverse-Engineered Internals

> [!NOTE]
> **EXE Contains Inlined Subsystems**
> The retail `ForgedAlliance.exe` statically links or inlines the content of the engine DLLs (like `MohoEngine.dll`). The addresses below are actual runtime addresses within the executable that can be hooked/patched directly in memory.

> **Binary**: `ForgedAlliance.exe` v3599 (Supreme Commander: Forged Alliance)
> All addresses are absolute EXE addresses.
> Verified via Ghidra decompilation + FA-Binary-Patches repo.

---

## 1. Named Functions (Ghidra-Verified)

These functions have been decompiled and verified in Ghidra.

### Simulation Core

| Address | Name | Description |
|---------|------|-------------|
| `0x00749F40` | `Sim_SimBeat` | Main tick function – increments beat/tick counters, runs Lua GC |
| `0x0073DAD0` | `Sim_SimSync` | Hash sync for desync detection – reads simHashes ring buffer |
| `0x00746280` | `Sim_LogPrintf` | Sim-internal logging (`"beat %d"`, `"tick number %d"`) |
| `0x007434D0` | `SimCreate` | Creates and initializes the Sim object |
| `0x007433B0` | `SimAlloc` | Allocates Sim memory (0xAF8 bytes) |
| `0x00744060` | `SimSetup` | Sets up the Sim with map/rules |
| `0x007458E0` | `SimFinalize` | Destroys the Sim object |

### SimBeat Decompiled (0x00749F40)

```c
void SimBeat(Sim* sim) {
    LogPrintf("********** beat %d **********", sim->beatCounter);  // +0x8F8
    sim->tickNumber++;                                              // +0x900
    if (sim->tickNumber % 70 == 0)
        lua_gc(sim->luaState, 0);  // +0x8D8
    sim->beatProcessed = 1;        // +0x8FC
}
```

### SimSync Decompiled (0x0073DAD0)

```c
void SimSync(CSimDriver* driver) {
    Sim* sim = driver->sim;
    int beat = sim->beatCounter;         // +0x8F8
    int hashIndex = beat & 0x7F;         // 128-entry ring
    MD5Digest* hash = &sim->simHashes[hashIndex * 16];  // +0xB0
    if (hash[0] != 0 || hash[1] != 0 || hash[2] != 0 || hash[3] != 0)
        SendHashToNetwork(driver + 0x28, hash, beat);
}
```

### Session Management

| Address | Name | Description |
|---------|------|-------------|
| `0x00893160` | `CWldSession_Create` | Creates the world session |
| `0x00894A10` | `CWldSession_DoBeat` | Main UI per-beat logic (processes SSyncData) |
| `0x008942B0` | `CWldSession_TogglePause` | Toggles pause (SP: direct, MP: broadcast) |
| `0x008979A0` | `Lua_SessionIsPaused` | Lua binding for pause check |
| `0x00897800` | `Lua_SessionRequestPause` | Lua binding for pause request |
| `0x00880F00` | `CSaveGameRequestImpl_Init` | Save game request initialization |
| `0x008807F0` | `CSaveGame_CreateSinglePlayerSession` | Initialize SP from save |
| `0x0088C000` | `DoLoading` | Session loading phase |
| `0x00885DE0` | `WorldSessionLoad` | World session load |
| `0x0088BBB0` | `WLD_SetupSessionInfo` | Setup SP/MP session structure |
| `0x00876A20` | `Lua_LaunchReplaySession` | Lua binding to setup replay (`VCR_SetupReplaySession` equivalent) |

### Checksum Verification (0x007487C0)

```c
void Sim_VerifyChecksum(Sim* sim, int sourceId, int beat) {
    if (sim->commandSource == 0xFF) return;  // spectators skip
    int current = sim->beatCounter;
    if (beat < current - 128) return;        // too old
    if (beat >= current) return;             // future
    // compare localHash vs remoteHash, log mismatch if different
}
```

### Lobby & Networking

| Address | Name | Description |
|---------|------|-------------|
| `0x007C38C0` | `CLobby_LaunchGame` | Validates peers, assigns slots, creates ClientManager |
| `0x007C5CA0` | `CLobby_OnConnectionMade` | Handles peer transitions (CONNECTING→CONNECTED, LISTENING→GAME_ACTIVE) |
| `0x007C5ED0` | `CLobby_OnConnectionLost` | Handles reconnect routing (6-state machine) |
| `0x007C7AC0` | `CLobby_EjectPeer` | Remove peer from lobby |
| `0x007C7FC0` | `CLobby_SendSystemMessage` | Send system message to UI |
| `0x007C4E80` | `AssignClientIndex` | Assign client index in lobby |
| `0x007C4F60` | `AssignCommandSource` | Assign command source vector |

### Client Management

| Address | Name | Description |
|---------|------|-------------|
| `0x0053E400` | `CreateCReplayClient` | Allocates CReplayClient (0x160 bytes) |
| `0x0053BA50` | `InitCReplayClient` | Sets up vtable + replay stream |
| `0x0053E180` | `CreateCLocalClient` | Allocates local client |
| `0x0053BD40` | `InitCClientBase` | Base client initialization |
| `0x0053C21E` | `CClientBase_ProcessMessage` | Processes byte protocol 0x00-0xCB |
| `0x0053FAF0` | `CreateCClientManager` | Allocates CClientManagerImpl (0x184D0 bytes) |
| `0x0053DF20` | `InitCClientManager` | Initialize client manager |
| `0x0053E590` | `SetSimRate` | Set simulation rate |
| `0x0053E720` | `GetSimRate` | Get simulation rate |
| `0x0053E7E0` | `GetSimRateRequested` | Get requested rate |
| `0x0053F440` | `CClientManager_HandleEjectRequest` | In-game eject validation |
| `0x0055AE10` | `CalcMaxSimRate` | Calculate maximum simulation rate |

#### HandleEjectRequest Decompiled (0x0053F440)

```c
void HandleEjectRequest(CClientManagerImpl* mgr, int clientIndex, int requester) {
    IClient* client = mgr->mClients[clientIndex];   // +0x420
    if (requester != mgr->localClient) {             // +0x430
        mgr->mConnector->EjectClient(client);        // +0x418
    } else {
        Log("Ignoring eject request from %s for invalid client index %u");
    }
}
```

### Entity & Unit Systems

| Address | Name | Description |
|---------|------|-------------|
| `0x007489E0` | `CreateUnit` | Create a new unit |
| `0x006A53F0` | `InitUnit` | Initialize unit (0x6A8 bytes) |
| `0x006A5320` | `DestroyUnit` | Destroy unit |
| `0x006A0FB0` | `CreateProjectile` | Create projectile |
| `0x0069AFE0` | `InitProjectile` | Initialize projectile |
| `0x006FB3B0` | `CreateProp` | Create prop |
| `0x006F9D90` | `InitProp` | Initialize prop |
| `0x00677C90` | `InitEntity` | Initialize entity base class |
| `0x00707BF0` | `Internal_IsNeutral` | Check neutral relation |
| `0x005BD630` | `Internal_IsAlly` | Check ally relation |
| `0x005D5540` | `Internal_IsEnemy` | Check enemy relation |
| `0x006856C0` | `SimFindEntityChainById` | Find entity in sim by ID |
| `0x00898DC0` | `UserFindEntityChainById` | Find entity in user by ID |
| `0x008B0180` | `GiveOrder` | Issue order to units |
| `0x008B05E0` | `ISSUE_Command` | Issue command to units |
| `0x005D32B0` | `CAiSteeringImpl::Update` | Per-unit steering tick (__thiscall, ECX=this) |
| `0x005D32BB` | Hook site (v22)           | 6-byte patch site, return to 0x005D32C1       |

### Army Management

| Address | Name | Description |
|---------|------|-------------|
| `0x006FE530` | `SimArmyAlloc` | Allocate SimArmy |
| `0x006FE690` | `SimArmyCreate` | Create SimArmy (0x288 bytes) |
| `0x006FE670` | `DestroySimArmy` | Destroy SimArmy |
| `0x00707D60` | `GetSimArmy` | Get SimArmy by index |
| `0x0073B1B0` | `SetArmyIndex` | Set army index on CSimDriver |
| `0x008965E0` | `Internal_SetFocusArmy` | Set focused army |
| `0x00896670` | `Test_SetFocusAccessRights` | Validate focus change |
| `0x004035F0` | `IsValidCommandSource` | Validate command source |

### Blueprint System

| Address | Name | Description |
|---------|------|-------------|
| `0x00528460` | `RRuleGameRulesAlloc` | Allocate game rules |
| `0x00529120` | `RRuleGameRulesInit` | Initialize game rules |
| `0x00529510` | `DestroyRRuleGameRules` | Destroy game rules |
| `0x0050DD60` | `InitRBlueprint` | Initialize blueprint base |
| `0x005289D0` | `RegisterBlueprint` | Register blueprint category |
| `0x00531D80` | `CreateRUnitBlueprint` | Create unit blueprint |
| `0x0051E480` | `InitRUnitBlueprint` | Initialize unit blueprint (0x568 bytes) |
| `0x00532080` | `CreateRPropBlueprint` | Create prop blueprint |
| `0x00532380` | `CreateRProjectileBlueprint` | Create projectile blueprint |

### Camera & Rendering

| Address | Name | Description |
|---------|------|-------------|
| `0x007AA9C0` | `CreateCamera` | Create camera |
| `0x007A7950` | `InitCamera` | Initialize camera (0x858 bytes) |
| `0x007A7DC0` | `DestroyCamera` | Destroy camera |
| `0x007FA230` | `CreateWRenViewport` | Create render viewport |
| `0x007F90D0` | `TerrainRender` | Render terrain |
| `0x007EF9B0` | `DrawRings` | Draw range rings |
| `0x0081C660` | `DrawVision` | Draw vision overlay |

### Memory & Utility

| Address | Name | Description |
|---------|------|-------------|
| `0x00957A70` | `malloc+1` | Memory allocator |
| `0x00A82130` | `shi_new+1` | C++ new |
| `0x00957AF0` | `free+1` | Memory free |
| `0x009C4940` | `AbortF` | Fatal abort with format |
| `0x00937CB0` | `LogF` | Log with format |
| `0x00937D30` | `WarningF` | Warning with format |
| `0x00938E00` | `Format` | String format |

### Lua Integration

| Address | Name | Description |
|---------|------|-------------|
| `0x009133A0` | `luaG_typeerror` | Lua type error |
| `0x009274D0` | `luaH_getstr` | Lua hash table string lookup |
| `0x00914E90` | `luaF_newCclosure` | Create C closure |
| `0x009142A0` | `CallCFunctionFromLua` | Call C function from Lua |
| `0x009240A0` | `GetLuaState` | Get LuaState from lua_State |
| `0x004CD3A0` | `RegisterLuaCFunction` | Register Lua C function |
| `0x004CE020` | `DoFile` | Execute Lua file |

---

## 2. Global Pointers (EXE)

| Address | Name | Type | Notes |
|---------|------|------|-------|
| `0x10C4F50` | `g_CSimDriver` | `CSimDriver*` | Already in `global.h` |
| `0x10C4F58` | `g_SWldSessionInfo` | `SWldSessionInfo*` | Already in `global.h` |
| `0x10A6470` | `g_CWldSession` | `CWldSession*` | Already in `global.h` |
| `0x10A63F0` | `g_Sim` | `Sim*` | Already in `global.h` |
| `0x10A6450` | `g_CUIManager` | `CUIManager*` | Already in `global.h` |
| `0x10A67B8` | `g_EngineStats` | `EngineStats*` | Already in `global.h` |
| `0x10A6478` | `g_ConsoleLuaState` | `LuaState*` | Already in `global.h` |
| `0x1290710` | (copy) | `uint32_t` | Copy of `CSimDriver.beatCounter2` |

---

## 3. Replay File Format

```
Offset  Content
0x00    ":Replay v1.9\r\n"       (13 bytes magic)
0x0D    [4] mapNameLength
0x11    [N] mapName
        [4] scenarioNameLength
        [N] scenarioName
        [1] armyCount
          per army: [N+1] name (null-term) + [4] data
        [1] extraFlag
        [1] modCount
          per mod: [4] nameLen + [N] name + commands (0xFF term)
        [4] randomSeed
```

---

## 4. Session States

| Value | State | Description |
|-------|-------|-------------|
| 0 | None | No active session |
| 1 | Loading | Session is loading |
| 2 | Started | Session started |
| 3 | SIM Initialized | Simulation initialized |
| 4 | SIM Started | Simulation running |
| 5 | Game Started | Game is active |
| 7 | Restart Requested | Restart pending |
| 8 | Session Halted | Session stopped → WLD_Teardown |

---

## 5. Game Types

| Function | Type |
|----------|------|
| `CLobby::LaunchGame` | Multiplayer |
| `VCR_SetupReplaySession` | Replay |
| `WLD_SetupSessionInfo` | SinglePlayer |
| `CSavedGame::CreateSinglePlayerSession` | Saved Game |
