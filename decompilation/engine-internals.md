# Engine Internals – New Findings from Ghidra Analysis

> Supplements existing `Info.txt`, `moho.h`, and `global.h`.
> Only contains information **not already present** in this repository.
> All addresses verified via live Ghidra decompilation of `ForgedAlliance.exe` (v3599),
> `MohoEngine.dll`, `gpgcore.dll`, `LuaPlus_1081.dll`, and `gpggal.dll`.

---

## 1. MohoEngine.dll – Session Lifecycle (WLD_*)

The complete session lifecycle is exposed as DLL exports with full C++ mangled names:

| Export | DLL Address | C++ Signature |
|--------|-------------|---------------|
| `WLD_SetupSessionInfo` | `0x104500D0` | `void (SWldSessionInfo&)` |
| `WLD_CreateSession` | `0x10459920` | `void ()` |
| `WLD_BeginSession` | `0x10450560` | `void ()` |
| `WLD_GetSession` | `0x104599F0` | `CWldSession* ()` |
| `WLD_GetDriver` | `0x10450760` | `ISTIDriver* ()` |
| `WLD_GetSessionLoader` | `0x10448D00` | |
| `WLD_IsSessionActive` | `0x1044F710` | `bool ()` |
| `WLD_Frame` | `0x1044FFE0` | `void ()` |
| `WLD_SetGameSpeed` | `0x10450720` | |
| `WLD_GetSimRate` | `0x104505B0` | |
| `WLD_IncreaseSimRate` | `0x10450670` | |
| `WLD_DecreaseSimRate` | `0x104506E0` | |
| `WLD_ResetSimRate` | `0x104506B0` | |
| `WLD_RequestEndSession` | `0x1044F720` | |
| `WLD_RequestRestartSession` | `0x1044F740` | |
| `WLD_DestroySession` | `0x104599A0` | |
| `WLD_Teardown` | `0x1044FD80` | |
| `WLD_LoadScenarioInfo` | `0x1044F5D0` | |
| `WLD_LoadMapPreview` | `0x104547D0` | |
| `WLD_CreateProps` | `0x10455280` | |
| `WLD_SetUIProvider` | `0x1044F5A0` | |

---

## 2. Sim Singleton – Full API (NEW)

The `Sim` class exposes a complete API via MohoEngine.dll exports. `Sim::Get()` is a **static singleton accessor**.

```cpp
static Sim*           Sim::Get();                        // 0x101334B0
LuaState*             Sim::GetLuaState();                // 0x10133590
uint                  Sim::GetCurrentTick();              // 0x101335B0
uint                  Sim::GetArmyCount();                // 0x101335E0
SimArmy*              Sim::GetArmy(uint index);            // 0x10133600
STIMap*               Sim::GetMap();                      // 0x10133550
ISimResources*        Sim::GetResources();                // 0x10133570
CRandomStream*        Sim::GetRandom();                   // 0x101335C0
COGrid*               Sim::GetOGrid();                    // 0x101335D0
RRuleGameRules*       Sim::GetGameRules();                // 0x10133540
IEffectManager*       Sim::GetEffectManager();            // 0x10133520
ISoundManager*        Sim::GetSound();                    // 0x10133530
MD5Context&           Sim::GetChecksumContext();          // 0x101334E0  (mutable)
const MD5Context&     Sim::GetChecksumContext() const;    // 0x101334F0  (const)
MD5Digest             Sim::GetChecksumDigest() const;     // 0x10133500
void                  Sim::UpdateChecksum(void*, uint);    // 0x101334D0
bool                  Sim::IsLoggingActive() const;       // 0x101334C0
bool                  Sim::CheatsEnabledNoRecord() const; // 0x101335A0
```

### SIM_CreateDriver – Full Signature

```cpp
// 0x1030F7C0, allocates 0x230 bytes for ISTIDriver
ISTIDriver* SIM_CreateDriver(
    auto_ptr<IClientManager>    clientMgr,
    auto_ptr<Stream>            replayStream,
    shared_ptr<LaunchInfoBase>  launchInfo,
    unsigned int                flags
);
```

### CLIENT_CreateClientManager – Full Signature

```cpp
// 0x1012CD70, allocates 0x184D0 bytes
IClientManager* CLIENT_CreateClientManager(
    uint           clientCount,
    INetConnector* connector,
    int            port,
    bool           isHost
);
```

---

## 3. Sim Beat – Decompiled Internals (NEW)

Decompiled from EXE `0x00749F40`:

```c
void SimBeat(Sim* sim) {
    LogPrintf("********** beat %d **********", sim->beatCounter);  // +0x8F8
    sim->tickNumber++;                                              // +0x900
    if (sim->tickNumber % 70 == 0)
        lua_gc(sim->luaState, 0);      // +0x8D8 = lua_State*
    sim->beatProcessed = 1;            // +0x8FC
}
```

### Sim Object Offsets (Supplements moho.h)

| Offset | Field | Type | New? |
|--------|-------|------|------|
| `+0x0B0` | checksumHashes | `MD5Digest[128]` (ring buffer) | ✅ |
| `+0x8D8` | luaState | `lua_State*` | ✅ |
| `+0x8F8` | beatCounter | `int32` | ✅ |
| `+0x8FC` | beatProcessed | `bool` | ✅ |
| `+0x900` | tickNumber | `int32` | ✅ |
| `+0x908` | beatBase | `int32` (checksum window base) | ✅ |
| `+0x92C` | commandSource | `int` (0xFF = spectator) | ✅ |

---

## 4. Checksum & Desync – Internal Algorithms (NEW)

### GetBeatChecksum (DLL 0x103176D0)

```cpp
bool Sim::GetBeatChecksum(CSeqNo beat, MD5Digest& outHash) {
    if (beat - beatBase >= -128 && beat - beatBase < 0) {
        int idx = ((beat & 0x7F) + 12) * 16;
        memcpy(&outHash, (char*)this + idx, 16);
        return true;
    }
    return false;  // beat outside 128-beat window
}
```

### VerifyChecksum (EXE 0x007487C0)

```c
void Sim_VerifyChecksum(Sim* sim, int sourceId, int beat) {
    if (sim->commandSource == 0xFF) return;  // spectators skip
    int current = sim->beatCounter;
    if (beat < current - 128) return;        // too old
    if (beat >= current) return;             // future
    // compare localHash vs remoteHash, log mismatch
}
```

### SDesyncInfo – Full Constructor Signature

```cpp
SDesyncInfo(CSeqNo beat, int sourceId, const MD5Digest& localHash, const MD5Digest& remoteHash);
// DLL 0x10133390
```

### Checksum DLL Exports

| Export | Address |
|--------|---------|
| `Sim::GetBeatChecksum(CSeqNo, MD5Digest&)` | `0x103176D0` |
| `Sim::UpdateChecksum(void*, uint)` | `0x101334D0` |
| `CMarshaller::VerifyChecksum` | `0x102C1170` |
| `Sim::VerifyChecksum` | `0x103178A0` |
| `DecodeVerifyChecksum` | `0x102BF940` |

---

## 5. Network Protocol – Message Types (NEW)

Decoded from `CClientBase::ProcessMessage` at EXE `0x0053BF30`:

| Byte | Type | Description |
|------|------|-------------|
| `0x00` | Beat Data | Player commands for one beat |
| `0x32` | ACK | Beat acknowledgment per client |
| `0x33` | DISPATCHED | Beat was processed by remote |
| `0x34` | AVAILABLE | Beat data available from remote |
| `0x35` | Eject Pending | Sets pending flag (+0x54) |
| `0x36` | Eject Request | Triggers HandleEjectRequest |
| `0x37` | Chat Message | Routed via CClientManager+0x418 |
| `0x38` | Game Speed Request | Speed change (checked vs +0x454) |
| `0x39` | Set Sim Speed | Direct speed set (+0x35) |
| `0xCA` | Disconnect | Client disconnected |
| `0xCB` | Drop | Client dropped |

### Per-Client Beat Tracking (CClientBase)

| Offset | Field |
|--------|-------|
| `+0x28` | ackedBeat |
| `+0x2A` | availableBeatRemote |
| `+0x2C` | ackedBeats[] (per-remote) |
| `+0x2F` | dispatchedBeatRemote |
| `+0x35` | simSpeed |
| `+0x54` | ejectPending |

### CClientManagerImpl Beat Pipeline (NEW)

| Offset | Field | Description |
|--------|-------|-------------|
| `+0x438` | mDispatchedBeat | Last executed beat |
| `+0x43C` | mAvailableBeat | Last available beat |
| `+0x440` | mFullyQueuedBeat | All clients queued |
| `+0x444` | mPartiallyQueuedBeat | Some clients queued |
| `+0x448` | mGameSpeedClock | Speed timer |
| `+0x450` | mGameSpeed | Current speed |

---

## 6. NET_* – Full C++ Signatures (NEW)

| Export | Address | Signature |
|--------|---------|-----------|
| `NET_MakeConnector` | `0x100791A0` | `INetConnector* (ENetProtocol, ushort port, weak_ptr<INetNATTraversalProvider>)` |
| `NET_OpenDatagramSocket` | `0x10079940` | `INetDatagramSocket* (ushort port, INetDatagramHandler*)` |
| `NET_GetAddrInfo` | `0x1007A470` | `bool (StrArg host, ushort port, bool ipv4Only, uint& outIP, ushort& outPort)` |
| `NET_TCPConnect` | `0x1007CE00` | `INetTCPSocket* (uint IP, ushort port)` |
| `NET_GetDottedOctetFromUInt32` | `0x1007A6C0` | `string (uint)` |
| `NET_GetUInt32FromDottedOcted` | `0x1007A700` | `uint (string)` |
| `NET_GetProtocolName` | `0x10079250` | `string (ENetProtocol)` |
| `NET_ProtocolFromString` | `0x10079320` | `ENetProtocol (StrArg)` |
| `NETMAIL_SendError` | `0x10079B30` | `void (StrArg, const char*)` |
| `NETMAIL_CanSendError` | `0x10079B40` | `bool ()` |

### TCP Server Implementation (NEW)

| Export | Address |
|--------|---------|
| `CNetTCPServerImpl` ctor | `0x1007D060` |
| `CNetTCPServerImpl::Accept` | `0x1007D0E0` |
| `CNetTCPServerImpl::GetLocalPort` | `0x1007D0B0` |

### Connection Callbacks (NEW DLL addresses)

| Export | Address |
|--------|---------|
| `OnConnectionMade` | `0x1038DDA0` |
| `OnConnectionLost` | `0x1038DFA0` |
| `OnConnectionFailed` | `0x1038DC90` |
| `OnConnectionClosed` | `0x1038E3C0` |

### CNetUDPConnection Layout (from DebugDump, NEW)

| Offset | Field | Type |
|--------|-------|------|
| `+0x42C` | connectionState | enum (6 states) |
| `+0x4A0` | nextSerial | uint16 |
| `+0x4B0` | nextSeqNum | uint16 |
| `+0x4B4` | expectedSeqNum | uint16 |
| `+0xD88` | pingTime | float |
| `+0xE48` | totalBytesQueued | uint64 |
| `+0xE50` | totalBytesSent | uint64 |
| `+0xE58` | totalBytesRecv | uint64 |

---

## 7. Pause/Resume – Decompiled Code (NEW)

### CWldSession::RequestPause (DLL 0x10456AB0)

```cpp
void CWldSession::RequestPause() {
    if (!isMultiplayer) {            // +0x484
        simDriver->vtable[0x40]();   // pause sim thread
        pauseActive = 1;             // +0x465
        pauseFlag = 1;               // +0x466
        pauseRequestBeat = beat;     // +0x468
    } else if (!mpPause) {           // +0x46C
        mpPause = 1;
        simDriver->vtable[0x14]();   // broadcast to peers
    }
    Broadcaster<SPauseEvent>::BroadcastEvent(this+8, true);
}
```

### CWldSession Pause Offsets (NEW)

| Offset | Field | Mode |
|--------|-------|------|
| `+0x464` | spPause | SP state |
| `+0x465` | pauseActive | SP active flag |
| `+0x466` | pauseFlag | SP value |
| `+0x468` | pauseRequestBeat | Beat when paused |
| `+0x46C` | mpPause | MP state |
| `+0x484` | isMultiplayer | SP/MP switch |

### Pause Export Chain (DLL addresses, NEW)

| Export | Address |
|--------|---------|
| `CWldSession::RequestPause` | `0x10456AB0` |
| `CWldSession::Resume` | `0x10456B30` |
| `CWldSession::IsPaused` | `0x10456BB0` |
| `CMarshaller::RequestPause` | `0x102C1270` |
| `CMarshaller::Resume` | `0x102C1350` |
| `Sim::RequestPause` | `0x10317A40` |
| `Sim::Resume` | `0x10317A80` |

---

## 8. Lobby Disconnect State Machine (NEW)

Decompiled from `CLobby::OnPeerDisconnect` at EXE `0x7C5ED0`:

```
State 1/2: Lobby DC      → "retrying" (active reconnect)
State 3:   Waiting        → "waiting for reconnect" (passive)
State 4:   In-Game DC     → retry or wait
State 5:   In-Game DC     → EJECT (host) or WAIT
State 6:   Ejected        → final, no recovery
```

### Peer Management (DLL addresses, NEW)

| Export | Address |
|--------|---------|
| `AddPeer` | `0x1038F1B0` |
| `DeletePeer` | `0x1038F700` |
| `EjectPeer` | `0x1038F8F0` |
| `FindPeer` | `0x10390170` |
| `GetPeer` | `0x1038ADB0` |
| `GetPeers` | `0x1038ACF0` |
| `GetPeerCount` | `0x1038A070` |
| `OnNewPeer` | `0x1038F050` |
| `OnDeletePeer` | `0x1038F600` |
| `OnEstablishedPeers` | `0x1038FB50` |
| `SendPeerInfo` | `0x1038FFA0` |
| `AssignClientIndex` | `0x1038D150` |
| `AssignCommandSource` | `0x1038D240` |
| `MakePeerTable` | `0x1038AE80` |
| `LaunchGame` | `0x1038BD40` |
| `OnRejected` | `0x1038EB20` |

---

## 9. Replay File Format – Byte-Level (NEW)

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

### Replay Exports (DLL, NEW)

| Export | Address |
|--------|---------|
| `VCR_CreateReplay` | `0x1043A090` |
| `VCR_SetupReplaySession` | `0x1043A940` |
| `IsReplay` | `0x10246260` |
| `BecomeObserver` | `0x10458D00` |
| `IsObserver` | `0x10458CD0` |

---

## 10. CDecoder – Command Decode Pipeline (NEW DLL addresses)

| Function | DLL Address |
|----------|-------------|
| `DecodeMessage` | `0x102BF5E0` |
| `DecodeSetCommandSource` | `0x102BF900` |
| `DecodeCommandSourceTerminated` | `0x102BF930` |
| `DecodeVerifyChecksum` | `0x102BF940` |
| `DecodeRequestPause` | `0x102BF9D0` |
| `DecodeResume` | `0x102BF9E0` |
| `DecodeSingleStep` | `0x102BF9F0` |
| `DecodeAdvance` | `0x102BF8C0` |
| `DecodeCreateUnit` | `0x102BFA00` |
| `DecodeCreateProp` | `0x102BFB10` |
| `DecodeDestroyEntity` | `0x102BFBC0` |
| `DecodeWarpEntity` | `0x102BFBF0` |
| `DecodeIssueCommand` | `0x102BFD60` |
| `DecodeIssueFactoryCommand` | `0x102BFE70` |
| `DecodeExecuteLuaInSim` | `0x102C01B0` |
| `DecodeLuaSimCallback` | `0x102C0260` |
| `DecodeEndGame` | `0x102C0470` |

### ExecuteLuaInSim (DLL 0x102C24F0)

```cpp
void CMarshaller::ExecuteLuaInSim(StrArg funcName, LuaObject& args) {
    CMessageStream stream;
    stream.Write(funcName, strlen(funcName) + 1);
    SCR_ToByteStream(&args, &stream);
    clientManager->vtable[0x44](&stream);
}
```

---

## 11. GpgNet Protocol (DLL addresses, NEW)

| Export | Address | Signature |
|--------|---------|-----------|
| `GPGNET_Attach` | `0x10382210` | |
| `GPGNET_ReportDesync` | `0x10382800` | `(int tickA, int tickB, string& hashA, string& hashB)` |
| `GPGNET_ReportBottleneck` | `0x10382340` | |
| `GPGNET_ReportBottleneckCleared` | `0x10382760` | |
| `GPGNET_SubmitArmyStats` | `0x103829A0` | |
| `GPGNET_Shutdown` | `0x10382A80` | |

---

## 12. Console System (DLL, NEW)

| Export | Address |
|--------|---------|
| `CON_Execute` | `0x1001C4E0` |
| `CON_Executef` | `0x1001C810` |
| `CON_ExecuteSave` | `0x1001C960` |
| `CON_ExecuteLastCommand` | `0x1001CA60` |
| `CON_FindCommand` | `0x1001CAC0` |
| `CON_GetCommandList` | `0x1001BF30` |
| `CON_ParseCommand` | `0x1001B980` |
| `CON_UnparseCommand` | `0x1001BDF0` |
| `CON_Printf` | `0x1001C2A0` |
| `CON_AddOutputHandler` | `0x1001C1C0` |
| `CON_RemoveOutputHandler` | `0x1001C200` |

### Console Variable Types

`CConCommand`, `CConFunc`, `CConAlias`, `TConVar<bool>`, `TConVar<int>`, `TConVar<unsigned char>`

---

## 13. Message Dispatch (DLL, NEW)

| Export | Address |
|--------|---------|
| `CMessageDispatcher` ctor | `0x10076830` |
| `CMessageDispatcher::Dispatch` | `0x10076AF0` |
| `CMessageDispatcher::PushReceiver` | `0x10076960` |
| `CMessageDispatcher::RemoveReceiver` | `0x10076A10` |

---

## 14. UI System (DLL, NEW)

| Export | Address |
|--------|---------|
| `UI_Init` | `0x10411CD0` |
| `UI_StartGame` | `0x10403A70` |
| `UI_StartFrontEnd` | `0x10403970` |
| `UI_StartHostLobby` | `0x10403750` |
| `UI_StartJoinLobby` | `0x10403860` |
| `UI_Beat` | `0x10411DA0` |
| `UI_LuaBeat` | `0x10404410` |
| `UI_Frame` | `0x10411D80` |
| `UI_ShowDesyncDialog` | `0x10403D30` |
| `UI_UpdateDisconnectDialog` | `0x10403C50` |
| `UI_NoteGameOver` | `0x10403F70` |
| `UI_NoteGameSpeedChanged` | `0x10403EA0` |
| `UI_ToggleConsole` | `0x10404040` |
| `UI_OnCommandIssued` | `0x10404E90` |

---

## 15. Threading (DLL, NEW)

| Export | Address | Signature |
|--------|---------|-----------|
| `THREAD_GetMainThreadId` | `0x10011A20` | `uint ()` |
| `THREAD_IsMainThread` | `0x10011A00` | `bool ()` |
| `THREAD_InvokeAsync` | `0x10011AC0` | `void (function<void()>, uint)` |
| `THREAD_InvokeWait` | `0x10011BA0` | `void (function<void()>, uint)` |
| `THREAD_SetAffinity` | `0x10012100` | `void (bool)` |

---

## 16. Scripting Infrastructure (DLL, NEW)

| Export | Address |
|--------|---------|
| `SCR_LuaDoFile` | `0x100BF9B0` |
| `SCR_LuaDoString` | `0x100BF5F0` |
| `SCR_LuaDoScript` | `0x100C01F0` |
| `SCR_Import` | `0x100C3FF0` |
| `SCR_ToByteStream` | `0x100C3BE0` |
| `SCR_FromByteStream` | `0x100C3970` |
| `USER_GetLuaState` | `0x104876C0` |
| `USER_GetReplayDir` | `0x1048AA00` |
| `USER_GetReplayExt` | `0x1048AAA0` |
| `USER_GetSaveGameDir` | `0x1048A930` |
| `USER_GetSaveGameExt` | `0x1048A9D0` |
| `SIM_FromLuaState` | `0x10319EB0` |

---

## 17. DLL Global Pointers (NEW)

| Address | Name | Type |
|---------|------|------|
| `0x109D9684` | g_SessionPtr | `CWldSession**` |
| `0x109D2750` | g_GpgNetActive | `int` |
| `0x109D2754` | g_GpgNetInterface | `IGpgNet*` |

(Note: EXE globals `g_CSimDriver`, `g_Sim`, `g_CWldSession`, etc. are already in `global.h`)

---

## 18. Save/Load System (DLL, NEW)

| Export | Address |
|--------|---------|
| `ApplyPendingSaveData` | `0x10457FA0` |
| `GetSaveData` | `0x10459660` |
| `SaveState` | `0x10319E40` |
| `CSavedGame` ctor | `0x10443950` |

### Serialization Chain

```
CArmyImpl  → CArmyImplSerializer
CAiBrain   → CAiBrainSerializer
LuaState   → LuaStateSerializer → LuaObjectSerializer → TableSerializer → UpValSerializer
```

---

## 19. Terrain System (DLL, NEW)

| Export | Address | Signature |
|--------|---------|-----------|
| `CHeightField::GetElevation` | `0x1004C8B0` | `float (float x, float z)` |
| `CHeightField::SetElevation` | `0x1004CEA0` | `void (uint x, uint z, float h)` |
| `CHeightField::GetSlope` | `0x1004C7D0` | `float (int x, int z)` |
| `CHeightField::GetVertex` | `0x1004C980` | `Vector3 (int x, int z)` |
| `CHeightField::GetNormal` | `0x1004C9F0` | `Vector3 (float x, float z)` |
| `CHeightField::GetRawDataPointer` | `0x1004CF10` | `uint16* ()` |
| `CHeightField::CopyHeightsFrom` | `0x1004CF40` | `void (const CHeightField&)` |
| `CHeightField::GetHeightScale` | `0x1004CE90` | `static float ()` |

---

## 20. Blueprint System (DLL, new addresses)

| Export | Address |
|--------|---------|
| `RBlueprint::InitBlueprint(LuaObject)` | `0x100FB300` |
| `RBlueprint::GetLuaBlueprint(LuaState*)` | `0x100FB390` |
| `BP_ShortId(string)` | `0x100FB420` |
| `RULE_CreateGameRules(string)` | `0x10115990` |
| `RRuleGameRulesImpl::GetUnitBlueprints()` | `0x101158C0` |
| `RRuleGameRulesImpl::GetPropBlueprints()` | `0x101158D0` |
| `RRuleGameRulesImpl::GetProjectileBlueprints()` | `0x101158F0` |
| `RRuleGameRulesImpl::GetMeshBlueprints()` | `0x101158E0` |
| `RRuleGameRulesImpl::GetUnitCount()` | `0x10115900` |
| `RRuleGameRulesImpl::InitBlueprint` | `0x10116EC0` |
| `RRuleGameRulesImpl::SetupEntityCategories` | `0x10116FD0` |
| `RRuleGameRulesImpl::ResolveCategoryReferences` | `0x10117210` |
| `RRuleGameRulesImpl::AssignNextOrdinal` | `0x10115890` |

---

## 21. Random Number Generation (DLL, NEW)

| Export | Address | Signature |
|--------|---------|-----------|
| `CRandomStream::IRand()` | `0x1000CE30` | `uint ()` |
| `CRandomStream::IRand(uint max)` | `0x1000CE40` | `uint (uint)` |
| `CRandomStream::IRand(int min, int max)` | `0x1000CE60` | `int (int, int)` |
| `CRandomStream::FRandGaussian()` | `0x1000D1F0` | `float ()` |
| `CRandomStream::Checksum(MD5Context&)` | `0x1000D360` | |
| `CMersenneTwister::Seed(uint)` | `0x1000CF30` | |
| `CMersenneTwister::Checksum(MD5Context&)` | `0x1000D030` | |

---

## 22. File System (DLL, NEW)

| Export | Address |
|--------|---------|
| `FILE_IsSystem` | `0x1000DCE0` |
| `FILE_IsLocal` | `0x1000DD00` |
| `FILE_IsAbsolute` | `0x1000E0F0` |
| `FILE_MakeAbsolute` | `0x1000E320` |
| `FILE_Dir` | `0x1000EF50` |
| `FILE_Ext` | `0x1000E9B0` |
| `FILE_Base` | `0x1000F530` |
| `FILE_Wild` | `0x1000F650` |
| `FILE_CollapsePath` | `0x1000F910` |

---

## 23. Entity & Army (DLL, NEW)

| Export | Address |
|--------|---------|
| `AddEntity` | `0x10456940` |
| `LookupEntityId` | `0x10456A80` |
| `OrphanEntity` | `0x104569B0` |
| `CMarshaller::DestroyEntity` | `0x102C1780` |
| `Sim::DestroyEntity` | `0x10317D40` |
| `RequestFocusArmy` | `0x10458D20` |
| `ValidateFocusArmyRequest` | `0x10458DA0` |
| `GetFocusArmy` | `0x10133840` |

---

## 24. Application Lifecycle (DLL, NEW)

| Export | Address |
|--------|---------|
| `WIN_AppExecute` | `0x100E1250` |
| `WIN_AppRequestExit` | `0x100E15B0` |
| `WIN_GetCurrentApp` | `0x100E15C0` |
| `WIN_ShowCrashDialog` | `0x100E09D0` |
| `WIN_CrashDialogDieHandler` | `0x100E0CD0` |
| `IWinApp::AppInitCommonServices` | `0x100E0D30` |
| `CFG_GetArgOption` | `0x1001B090` |
| `CFG_GetArgs` | `0x1001B190` |
| `STAT_Frame` | `0x10016320` |
| `STAT_GetLuaTable` | `0x1001AF50` |

---

## 25. WLD_Frame – Session State Machine (NEW, DLL 0x1044FFE0)

`WLD_Frame` is the **main game loop dispatcher**. It switches on a global state variable `DAT_109ba854` (= `g_SessionState`):

```c
bool WLD_Frame(float deltaTime) {
    // Pre-frame update via vtable
    (*sessionVtable[0x14])();

    do {
        bool continueLoop = false;
        switch (g_SessionState) {
            case 0: (*sessionVtable[0])();  return true;  // Idle
            case 1: DoLoading();             return true;  // Loading
            case 2: DoStarted(&continueLoop);  break;     // Started
            case 3: DoSimInit(&continueLoop);   break;     // SIM Initialized
            case 4: DoSimStarted();             break;     // SIM Started
            case 5: DoGameRunning(&continueLoop); break;   // Game Running
            case 6: DoGameFrame(deltaTime);  return true;  // Active Frame
            case 7: DoRestartRequested();    return true;  // Restart
            case 8: WLD_Teardown();          
                    UI_StartFrontEnd();      return true;  // Teardown → FrontEnd
        }
    } while (continueLoop);
    return true;
}
```

### Session State Globals (NEW, verifiziert)

| DLL Address | Name | Type | Description |
|-------------|------|------|-------------|
| `0x109ba854` | `g_SessionState` | `int` | Current session state (0-8) |
| `0x109d968c` | `g_SessionInfoPtr` | `SWldSessionInfo*` | Points to active session info |
| `0x109ba834` | `g_GpgNetPtr` | `void*` | GpgNet interface pointer |
| `0x109dbf74` | `g_UIBeatCounter` | `void*` | UI-side beat counter object |

### WLD_BeginSession (DLL 0x10450560)

```c
void WLD_BeginSession(auto_ptr<SWldSessionInfo> sessionInfo) {
    if (g_SessionInfoPtr != NULL && g_SessionInfoPtr != sessionInfo) {
        DestroySessionInfo(g_SessionInfoPtr);  // cleanup old
    }
    g_SessionInfoPtr = sessionInfo;
    g_SessionState = 1;  // → Loading
}
```

---

## 26. CLobby Connection Callbacks – Complete Decompilation (NEW)

Source file confirmed: `c:\work\rts\main\code\src\user\Lobby.cpp`

### SPeer (CLobbyPeer) – Verified Offsets (NEW)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| `+0x08` | playerName | `string` (0x1C) | Player name |
| `+0x24` | retryCount | `int` | Connection retry counter |
| `+0x28` | address | `uint` | Remote address for Connect/Listen |
| `+0x2C` | port | `ushort` | Remote port |
| `+0x30` | state | `int` | Peer state (1-6, see below) |
| `+0x38` | connection | `INetConnection*` | Active connection object |
| `+0x4C` | cmdSourceIndex | `int` | Command source (-1 = unassigned) |

### Peer States (Complete, from Decompilation)

```
State 1: CONNECTING     – Actively connecting (via Connect)
State 2: CONNECTED      – Connection established (lobby phase)
State 3: LISTENING      – Passively listening for reconnect
State 4: GAME_ACTIVE    – In-game, connection alive
State 5: GAME_LAUNCHED  – Game launched, peer active
State 6: EJECTED        – Peer ejected, no recovery
```

### OnConnectionMade (DLL 0x1038DDA0)

```cpp
void CLobby::OnConnectionMade(INetConnection* conn) {
    SPeer* peer = FindPeerByConnection(conn);
    
    Log("LOBBY: connection to %s made, status=%s.", peer->name, peer->status);
    
    switch (peer->state) {
        case 1:  // Was CONNECTING
            peer->state = 2;  // → CONNECTED
            dispatcher.PushReceiver(conn+4, 100, 0x78, this+0x34);
            // Send our name + UID to the peer
            stream.Write(our_name, our_name.length + 1);
            stream.Write(&our_UID, 4);
            conn->vtable[0x10](stream);  // Send handshake
            break;
            
        case 3:  // Was LISTENING (reconnect succeeded!)
            peer->state = 4;  // → GAME_ACTIVE
            dispatcher.PushReceiver(conn+4, 100, 0x78, this+0x34);
            break;
            
        default:
            ASSERT("Reached the supposably unreachable.", 0x640, "Lobby.cpp");
    }
}
```

### OnConnectionLost (DLL 0x1038DFA0) – Full State Machine

```cpp
void CLobby::OnConnectionLost(INetConnection* conn) {
    if (conn == this->host) {   // +0x88
        Log("LOBBY: host disconnected.");
        Fire("ConnectionFailed");
        return;
    }
    
    SPeer* peer = FindPeerByConnection(conn);
    
    switch (peer->state) {
        case 1: case 2:  // Lobby phase
            Log("LOBBY: connection to %s lost, retrying");
            peer->connection = connector->Connect(peer->address);
            peer->state = 1;  // → CONNECTING
            break;
            
        case 3:  // LISTENING – should never lose a listen socket
            ASSERT("Reached the supposably unreachable.", 0x660, "Lobby.cpp");
            break;
            
        case 4:  // GAME_ACTIVE
            Log("LOBBY: lost connection to %s, waiting for them to reconnect.");
            peer->state = 3;  // → LISTENING
            peer->connection = connector->Listen(peer->address);
            break;
            
        case 5:  // GAME_LAUNCHED
            this->disconnectFlag = 1;  // +0xB8
            if (this->host == NULL && this->hosted_or_joined) {
                // WE ARE HOST → EJECT
                Log("LOBBY: lost connection to %s, ejecting 'em.");
                Msgf(peer->name, localized_message);
                peer->state = 6;  // → EJECTED
                EjectPeer(this, peer, "Disconnected");
            } else {
                // NOT HOST → try reconnect
                Log("LOBBY: lost connection to %s, waiting for them to reconnect.");
                Msgf(peer->name, localized_message);
                if (peer->retryCount < this->maxRetries  // +0xAC
                    && this->host != NULL) {
                    peer->connection = connector->Connect(peer->address);
                    peer->state = 1;   // → CONNECTING (active retry)
                } else {
                    peer->connection = connector->Listen(peer->address);
                    peer->state = 3;   // → LISTENING (passive wait)
                }
            }
            break;
    }
    conn->vtable[0x1C]();  // Destroy old connection
}
```

### OnConnectionFailed (DLL 0x1038DC90)

```cpp
void CLobby::OnConnectionFailed(INetConnection* conn) {
    if (conn == this->host) {
        Log("LOBBY: connection to master failed -- giving up.");
        Fire("ConnectionFailed", "HostLeft");
        return;
    }
    SPeer* peer = FindPeerByConnection(conn);
    Log("LOBBY: connection to %s failed, retrying...");
    peer->connection = connector->Connect(peer->address, peer->port);
    // PushReceiver with priority=200, id=0xD2
    dispatcher.PushReceiver(peer->connection+4, 200, 0xD2, this+0x34);
}
```

### CLobby Offsets (NEW, verified from decompilation)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| `+0x20` | luaObject | `LuaObject` | Lua state object |
| `+0x34` | messageReceiver | `IMessageReceiver` | For CMessageDispatcher |
| `+0x78` | connectorFactory | `INetConnectorFactory**` | vtable+0x10=Connect, +0x18=Listen |
| `+0x84` | hosted_or_joined | `bool` | Has CLobby been used? |
| `+0x88` | host | `INetConnection*` | Host connection (NULL if we ARE host) |
| `+0x8C` | isHosted | `bool` | True if we hosted the game |
| `+0x90` | our_name | `string` | Our player name |
| `+0xAC` | our_UID | `int` | Our player UID / maxRetries |
| `+0xB0` | peer_list | `linked_list<SPeer>` | Doubly-linked peer list |
| `+0xB4` | peer_list_head | `SPeer*` | First peer in list |
| `+0xB8` | disconnectFlag | `bool` | Set on in-game disconnect |
| `+0xC4` | gameSpeedSetting | `int` | Copied to LaunchInfo |

---

## 27. LaunchGame – Complete Flow (NEW, DLL 0x1038BD40)

`CLobby::LaunchGame(LuaObject const& options)` – 781 lines decompiled.

### Sequence:

```
1. Parse "ScenarioFile" from options → WLD_LoadScenarioInfo()
2. Set ScenarioInfo.Options from Lua
3. Create LaunchInfoNew via Ordinal_251()
4. Copy gameMods string (offset +0x0C) and map string (+0x28)
5. Copy gameSpeedSetting from CLobby+0xC4 to LaunchInfo+0xA0
6. Check "CheatsEnabled" option → set LaunchInfo+0x88
7. Parse "Configurations.standard.teams" → check FFA vs team mode
8. Iterate player options:
   a. Get "Human" boolean
   b. Get UID, match against our_UID → set focusArmy
   c. Call AssignClientIndex() for each human player
   d. Call AssignCommandSource() → returns command source index
   e. Build validCommandSources bitset
   f. Store ArmyName in player options
9. Process "ExtraArmies" → create civilian entries
10. Parse "GameSpeed": "fast"→flags=4, "adjustable"→flags=1
11. CLIENT_CreateClientManager(peerCount, connector, flags, isHost)
12. For each peer in state 5 (GAME_LAUNCHED):
    - Remove old message receivers
    - Register connection with ClientManager
13. Get "RandomSeed" from options
14. Store SWldSessionInfo → g_SessionInfoPtr (DAT_109d968c)
15. Set g_SessionState=1 (via DAT_109ba854)
```

### Key Findings:

| Detail | Value |
|--------|-------|
| Source file | `Lobby.cpp` confirmed |
| Game speed flags | `0`=normal, `1`=adjustable, `4`=fast |
| CheatsEnabled | From Lua option "true" string compare |
| FocusArmy | Set when `peer.UID == our_UID` |
| GameTypes | FFA parsed via `_stricmp("FFA", type)` |
| ClientManager | Created with `CLIENT_CreateClientManager(count, connector, speedFlags, isHost)` |
| Peer processing | Only state=5 peers get registered with ClientManager |

---

## 28. VCR_SetupReplaySession – Full Decompilation (NEW, DLL 0x1043A940)

```cpp
auto_ptr<SWldSessionInfo> VCR_SetupReplaySession(StrArg replayPath) {
    // 1. Open replay file (checks extension ".fafreplay"/".scfareplay")
    FileStream* stream = OpenReplayFile(replayPath);
    if (!stream) return nullptr;

    // 2. Read & verify magic: ":Replay v1.9\r\n"
    char magic[14];
    stream->Read(magic, sizeof(magic));
    if (strcmp(magic, ":Replay v1.9\r\n") != 0) return nullptr;

    // 3. Parse header
    //    - Map name (length-prefixed string)
    //    - Scenario name (length-prefixed string)
    //    - Army count (1 byte)
    //    - Per army: name + data
    //    - Mod list
    //    - Random seed

    // 4. Create LaunchInfoNew
    LaunchInfoNew* info = new LaunchInfoNew();
    info->gameMods = parsedMods;       // +0x0C
    info->mapPath = parsedMapPath;     // +0x28
    info->isReplay = true;             // +0x24 = 0, +0x25 = 1
    
    // 5. Build SWldSessionInfo
    SWldSessionInfo* sessionInfo = new SWldSessionInfo();
    sessionInfo->map_name = mapName;
    sessionInfo->isReplay = true;          // +0x25
    sessionInfo->isBeingRecorded = false;  // +0x24
    sessionInfo->isMultiplayer = false;
    sessionInfo->ourCmdSource = 0xFF;      // +0x2C = observer
    
    // 6. Create client manager for replay
    sessionInfo->clientManager = CLIENT_CreateClientManager(
        2,        // 2 clients (local + replay)
        NULL,     // no network connector
        0,        // no speed flags
        true      // isHost = true
    );
    
    // 7. Register replay data as "Local" client
    clientManager->vtable[0x0C]();  // set up replay stream
    clientManager->vtable[0x04]();  // add "Local" client
    
    return sessionInfo;
}
```

### Key Finding: Replay uses `CLIENT_CreateClientManager(2, NULL, 0, true)`
- `clientCount=2` → local observer + replay stream
- `connector=NULL` → no network
- `port/flags=0` → no speed control
- `isHost=true` → replay viewer is always "host"

---

## 29. DoBeat – SSyncData Processing (NEW, DLL 0x10456D40)

`CWldSession::DoBeat(auto_ptr<SSyncData>)` – 692 lines, the **core per-beat UI update**.

### SSyncData Structure (Reconstructed from offsets)

| Offset | Field | Description |
|--------|-------|-------------|
| `[0]` | beatNumber | Current beat number |
| `[1]` | focusArmyIndex | Focus army for this beat |
| `[2]+[3]` | gameSpeed | Speed data (int + flag) |
| `[0x43-0x44]` | newArmies | Array of new UserArmy data (0x88 bytes each) |
| `[0x47-0x48]` | deletedEntities | Entity IDs to remove (0x160 bytes each) |
| `[0x4B-0x4C]` | newEntities | SCreateEntityParams (0x0C bytes each) |
| `[0x4F-0x50]` | newUserUnits | New UserUnit data (0x1C bytes each) |
| `[0x53-0x54]` | unitCommands | Command updates (0xD8 bytes each) |
| `[0x57-0x58]` | unitUpdates | Full unit state updates (0x238 bytes each) |
| `[0x5B-0x5C]` | entityDestroys | Entities to destroy |
| `[0x5F-0x60]` | entityMoves | Position/rotation updates |
| `[0x63-0x64]` | projectiles | Projectile updates (0x3C bytes each) |
| `[0x67-0x68]` | buildProgress | Build progress (0x78 bytes each) |
| `[0x6B-0x6C]` | destroyEffects | Destroy visual effects |
| `[0x72]` | hasSound | Sound update flag |
| `[0x7D-0x7E]` | cameraUpdates | Camera sync data (0x1C per cam) |
| `[0x81-0x82]` | armyUpdates | Army data updates (0x0C each) |
| `[0x89-0x8A]` | poseUpdates | Pose/animation updates (0x0C each) |
| `[0x8D-0x8E]` | miscUpdates | Misc updates (0x10 each) |
| `[0x91-0x92]` | desyncInfos | SDesyncInfo array (0x28 bytes each) |
| `[0x94]` | pauseState | -1 = not paused, other = paused source |
| `[0x95-0x9A]` | armyStats | For GPGNET_SubmitArmyStats() |
| `[0x9C]` | isGameOver | Game over flag |
| `[0x9F-0xA0]` | mapData | Updated map data |
| `[0xA2-0xA3]` | consoleMessages | CON_Printf messages (0x1C each) |
| `[0xA9-0xAC]` | sharedPtrs | Shared pointer updates |
| `[0x271]` | fogOfWar | `ren_FogOfWar` boolean |

### DoBeat Flow:

```
1. CTimeBarSection("Sync") — start profiling
2. Get camera, get SimDriver
3. Copy gameSpeed from SSyncData to CWldSession+0x458
4. If focusArmy changed → clear selection, update sound
5. Process camera sync data
6. Process unit commands (vtable calls)
7. Create new UserArmies from SSyncData
8. Process unit deletions
9. Create new UserEntities, UserUnits
10. Process unit commands, moves, destroys
11. Process build progress, projectile updates
12. Process pose/animation updates
13. Update Lua globals:
    - Set "PreviousSync" = current Sync
    - Set "Sync" = SCR_FromByteStream(SSyncData.luaStream)
14. Execute Lua: SCR_LuaDoString(syncScript)
15. Update pause state: CWldSession+0x464 = (pauseState != -1)
16. Update shared pointers at +0x414/+0x418/+0x41C/+0x420/+0x424/+0x428
17. Copy ren_FogOfWar from SSyncData+0x271
18. Print console messages
19. Process desync infos:
    - For each: GPGNET_ReportDesync(beatA, beatB, hashA, hashB)
    - Call UI_ShowDesyncDialog()
20. Check game over → UI_NoteGameOver(), optional auto-exit
21. Apply pending save data if any
22. Process orphaned entities
23. CheckForNecessaryUIRefresh()
24. GPGNET_SubmitArmyStats() if data present
```

### CWldSession Offsets (additional, from DoBeat)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| `+0x414` | sharedPtr1 | `void*` | Updated from SSyncData |
| `+0x418` | sharedPtr2 | `shared_ptr` | Updated from SSyncData |
| `+0x41C` | sharedPtr3 | `void*` | Updated from SSyncData |
| `+0x420` | sharedPtr4 | `shared_ptr` | Updated from SSyncData |
| `+0x424` | simResources | `CSimResources*` | Updated from SSyncData |
| `+0x428` | sharedPtr5 | `shared_ptr` | Updated from SSyncData |
| `+0x458` | gameSpeed | `int` | Current game speed |
| `+0x464` | isPaused | `bool` | SSyncData.pauseState != -1 |
| `+0x468` | pauseBeat | `int` | Beat when paused |
| `+0x474` | commandSourceNames | `void*` | Command source name table |
| `+0x488` | focusArmyIndex | `int` | Focus army |
| `+0x48C` | isGameOver | `bool` | Game over flag |
| `+0x4FC` | pendingSaveData | `void*` | For ApplyPendingSaveData() |

---

## 30. Sim::AdvanceBeat – Full Tick Pipeline (NEW, DLL 0x10318F50)

299 lines decompiled. This is the **core simulation tick**.

```cpp
void Sim::AdvanceBeat(int beatDelta) {
    Logf("********** beat %d **********", this->beatBase);  // +0x908
    
    // Resume Lua thread from last coroutine yield
    (*luaThread->vtable[8])(luaCoroutine);  // +0x8D8, +0x8E8
    
    // Optional: Set Lua debug hook
    if (g_LuaDebugEnabled) {   // DAT_109ba78c
        LuaState::SetHook(luaCoroutine, debugHookFn, 4, 0);
    }
    
    // Check if tick should advance
    if (!this->skipTick && (this->pauseTarget == -1 || !this->pauseFlag)) {
        // INCREMENT TICK
        this->tickNumber++;  // +0x910
        Logf("  tick number %d", this->tickNumber);
        
        // === UNIT TICK PHASE ===
        // Iterate ALL units across ALL armies
        CUnitIterAllArmies iter(this);
        for (Unit* unit : iter) {
            if (!unit->IsBeingBuilt()) {
                // Clear reclaim values
                unit[0x2E0] = 0; unit[0x2E4] = 0;
                unit[0x2D8] = 0; unit[0x2DC] = 0;
            }
            TickUnit(unit);
        }
        
        // === COMMAND SOURCE TICK ===
        for (auto& cmdSrc : commandSources) {  // +0x920
            cmdSrc->vtable[0x3C]();  // ProcessCommands
        }
        
        // Run 3x FUN_10009390 (scheduler?)
        
        // === AI BRAIN TICK ===
        // Tick AI brains with round-robin scheduling
        uint armyCount = commandSources.size();
        for (uint i = 0; i < armyCount; i++) {
            AIBrain* brain = commandSources[i]->GetBrain();
            if (i == tickNumber % armyCount) {
                brain->FullTick(armyCount);   // Full tick every N beats
            } else {
                brain->QuickTick();           // Quick tick otherwise
            }
        }
        
        // === EFFECT + PROP TICK ===
        effectManager->vtable[0x34]();   // +0x8D0
        propManager->vtable[0x10]();     // +0x990
        
        // === CLEANUP ===
        // Kill cleanup for dead units
        for (Unit* unit : allUnits) {
            if (unit[0x514] != 0) Unit::KillCleanup(unit);
        }
        
        // === ENTITY ADVANCE ===
        // Advance coordinates for entities in list +0xA6C-0xA70
        for (auto& entity : entityList) {
            Entity::AdvanceCoords(entity - 0x60);
        }
        
        // === EXTRA UNIT DATA ===
        // Collect SExtraUnitData at +0xA38
        // Uses ring buffer at +0xAB8 (queue)
        
        // === SHARED PTR SWAP ===
        // Swap shared ptrs: +0x97C/0x980 → +0x984/0x988
        
        this->beatProcessed = true;   // +0x8F5 = 1
        this->beatFlag = false;       // +0x8F4 = 0
    }
    
    // === DEFERRED COMMAND QUEUE ===
    // Process commands from ring buffer at +0xA5C-0xA68
    while (deferredCount > 0) {
        cmd = PopFromRingBuffer();
        cmd->vtable[0x80]();  // Execute deferred command
    }
    
    // === CHECKSUM ===
    effectManager->vtable[0x3C]();  // +0x8D0
    // Conditional checksum update based on tick interval
    uint beatBase = this->beatBase;  // +0x908
    if (beatBase % checksumInterval == 0) {
        UpdateChecksum(this);
    }
    
    // === OBSERVERS ===
    for (auto& observer : observerList) {  // +0x9B0-0x9B4
        observer->vtable[0xC](this);
    }
    
    this->beatProcessedFlag = true;  // +0x90C = 1
}
```

### Sim Object Offsets (additional, from AdvanceBeat)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| `+0x8D0` | effectManager | `IEffectManager*` | Effect/particle system |
| `+0x8D8` | luaThread | `void*` | Lua thread vtable |
| `+0x8E8` | luaCoroutine | `LuaState*` | Sim Lua coroutine state |
| `+0x8ED` | skipTick | `bool` | Skip current tick |
| `+0x8F0` | pauseTarget | `int` | -1 = no pause target |
| `+0x8F4` | beatFlag | `bool` | Cleared after tick |
| `+0x8F5` | beatProcessed | `bool` | Set after tick completes |
| `+0x908` | beatBase | `int` | Beat counter for logging |
| `+0x90C` | beatProcessedFlag | `bool` | Final beat-done flag |
| `+0x910` | tickNumber | `int` | Incrementing tick counter |
| `+0x920` | commandSources | `vector<void*>` | Command source vector |
| `+0x990` | propManager | `void*` | Prop tick manager |
| `+0x9AC` | unk_9AC | `void*` | Used by FUN_10344270 |
| `+0x9B0` | observerList | `linked_list` | Sim observers |
| `+0x9BC` | checksumSlots | `void**` | Checksum slot array |
| `+0xA38` | extraUnitData | `vector` | SExtraUnitData collection |
| `+0xA5C` | deferredCmdRing | `void**` | Deferred command ring buffer |
| `+0xA60` | ringSize | `uint` | Ring buffer size |
| `+0xA64` | ringHead | `uint` | Ring buffer head |
| `+0xA68` | ringCount | `int` | Items in ring buffer |
| `+0xA6C` | entityAdvList | `linked_list` | Entities needing coord advance |
| `+0xAB8` | unitDataQueue | `void*` | Unit data queue |

---

## 31. CMarshaller::AdvanceBeat (DLL 0x102C2760)

Simpler wrapper that serializes beat number and sends to the sim thread:

```cpp
void CMarshaller::AdvanceBeat(int beatNumber) {
    CMessageStream stream;
    stream.Write(&beatNumber, 4);
    // Send via ClientManager queue (vtable offset 0x44)
    this->clientManager->vtable[0x44](stream);
}
```

This confirms the beat pipeline:
```
CWldSession::DoBeat()
  → CMarshaller::AdvanceBeat(beatNum)    // DLL, serializes
    → Sim::AdvanceBeat(beatNum)          // DLL, actual tick
```
