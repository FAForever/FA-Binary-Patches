# Reverse-Engineering & Decompilation Progress (Incl. Decompilation Project)

## Initial Situation
This report analyzes the decompilation progress of the *Supreme Commander: Forged Alliance* engine, taking into account the new structural analyses found in the `decompilation\` folder.

## The Paradigm Shift
While the original `FA-Binary-Patches` repository operated purely as a **hooking and patching instrument** (symptom treatment), the new `decompilation\` folder signifies the beginning of a **genuine reverse-engineering project**.

**New Achievements:**
1. **Structural Documentation:** With `MohoEngine.dll.md` and `ForgedAlliance.exe.md`, the foundational architecture of the engine (classes, VTable offsets, global pointers) is now being statically documented.
2. **Deciphering the Inlining Technique:** The critical revelation was made and proven that the release version of `ForgedAlliance.exe` has all engine libraries (such as `MohoEngine.dll`) inlined. This explains why previous analyses were so difficult.
3. **Identification of Core Systems:** Key subsystems were located and named:
   - **Simulation Tick (Beat):** `Sim_AdvanceBeat`, `Sim_SimBeat`
   - **Network & Client CMarshaller:** `ProcessMessage`, `HandleEjectRequest`
   - **Replay & UI:** `VCR_SetupReplaySession`, `LaunchGame`
4. **Use-Case Orientation:** Instead of rummaging through code blindly, concrete development goals (Rejoin Function, Sim Limits) were established in `use-cases.md` and directly linked with the decompilation efforts.

## Conclusion & Progress (10 - 15%)
With the initiation of real decompilation documentation, we have laid the groundwork for reversing the entire engine. The pace of **structural reconstruction** has increased massively. 

We have moved past the stage of "blind poking" (setting assembly hooks without understanding the system). We are now beginning to truly understand the C++ class hierarchy and its VTables. To completely reverse the engine (*Moho*), future efforts must involve creating Ghidra Type Archives to consistently synchronize the discovered C++ structs and VTables and generate real pseudonymized C files.

---

## Detailed Reversing Ledger (Combined Knowledge)

By merging the structural discoveries from the community's base repository with the new systematic decompilation project, we now have a massive reference library for the entire *Forged Alliance* engine.

### Part A: Community Knowledge (Systematically Extracted)
A python-based extraction script successfully parsed the whole repository (`hooks\`, `include\`, `section\`) to prove exactly what the community has discovered so far:
* **Verified Struct Sizes:** 19 core C++ structs have their memory sizes exactly validated:
  * `Sim` (0xAF8 / 2808 bytes), `CWldSession` (0x508 / 1288 bytes), `CSimDriver` (0x230 / 560 bytes), `STIMap` (0x1548 / 5448 bytes).
  * `SimArmy` (0x288 / 648 bytes), `BaseArmy` (0x1DC / 476 bytes), `UserArmy` (0x210 / 528 bytes).
  * `LuaState` (0x34 / 52 bytes), `LuaObject` (0x14 / 20 bytes), and `TObject` (0x8 / 8 bytes).
* **Global Pointers:** 16 exact global state singletons have been located in `.data` memory:
  * `g_Sim` (0x10A63F0), `g_CWldSession` (0x10A6470), `g_CSimDriver` (0x10C4F50), `g_SWldSessionInfo` (0x10C4F58).
  * `g_EntityCategoryTypeInfo` (0x10C6E70), `g_CAiBrainTypeInfo` (0x10C6FA0).
* **Hardcoded Hook Targets:** Exactly **152 memory addresses** are currently hooked across 48 files. Notable interventions include:
  * `FixCollisions.cpp` (6 hooks for physics rewriting).
  * `DesyncFix.cpp` (12 hooks for network decode/encode bypassing).
  * `EnableConsoleCommands.cpp` (8 hooks) and UI hooks (`OnWindowMessage` at `0x00430C0E`).

### Part B: New Decompilation Discoveries (DLL & Architecture)
Building upon the community base, the new decompilation effort (`decompilation\`) systematically translates these structures into full C++ execution pipelines:

#### 1. `MohoEngine.dll` (Engine Blueprint)
Utilized the un-optimized DLL as a Rosetta Stone, documenting **over 100 functions** with their exact C++ signatures, parameters, and workflows:
* **The Beat Pipeline (Sim):** Completely decompiled the 20-step pipeline of `Sim::AdvanceBeat` (TickUnit, Lua GC, extra unit data, observer notifications).
* **The SSyncData Bridge (UI):** Decompiled the 692-line `CWldSession::DoBeat` function, proving how the Simulation thread sends UI updates (camera, health, build progress) to the User thread.
* **Network & Decoding:** Mapped all 17 network command decoders (`DecodeVerifyChecksum`, `DecodeIssueCommand`, `DecodeExecuteLuaInSim`).
* **Connection State Machine:** Reversed the 6-state peer connection logic (`CLobby` - CONNECTING -> CONNECTED -> GAME_ACTIVE -> EJECTED) including retry-counters and UDP socket interactions.

#### 2. `ForgedAlliance.exe` (Production Binary Inlining)
Identified and mapped the exact memory addresses for **over 70 newly documented core engine functions** directly inside the release executable:
* **Simulation Core:** `Sim_SimBeat` (0x00749F40), `Sim_SimSync` (0x0073DAD0), `Sim_VerifyChecksum` (0x007487C0).
* **Session Lifecycle:** `CWldSession_DoBeat` (0x00894A10), `CWldSession_TogglePause`, `WLD_SetupSessionInfo`.
* **Lobby & Networking:** `CLobby_LaunchGame` (0x007C38C0), `CLobby_OnConnectionMade`, `CLobby_OnConnectionLost`.
* **Client Protocol:** `CClientBase_ProcessMessage` (0x0053C21E), `CClientManager_HandleEjectRequest` (0x0053F440).
* **Entities:** `CreateUnit`, `InitUnit`, `CreateProjectile`, `GiveOrder`.
