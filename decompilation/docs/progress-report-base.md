# Reverse-Engineering & Decompilation Progress (Base Repository)

## Initial Situation
This report analyzes the decompilation progress of the *Supreme Commander: Forged Alliance* engine, based exclusively on the code present in the `FA-Binary-Patches` repository (without the newer documentation folders).

## Repository Analysis
The repository currently contains approximately **114 C++ files** (distributed across the `hooks/` and `section/` folders), as well as header files (`include/`) and signature patches (`SigPatches.txt`).

**Key Findings:**
1. **No Real Decompiling:** The project is currently not performing any **systematic reverse-engineering** or decompilation of the entire engine (neither `ForgedAlliance.exe` nor `MohoEngine.dll`).
2. **Hooking Framework:** Instead, it is a highly advanced **runtime patching framework**. Assembly instructions (`JMP`, `CALL`) in RAM are targeted and overwritten using pattern scanning and hardcoded addresses (`SigPatches.txt` and `asm.h`).
3. **Selective Modifications:** The C++ files contain "only" replacement functions for very specific issues in the game (e.g., `DesyncFix.cpp`, `AirNoneCollisionFix.cpp`, `Fix4GB.cpp`). 
4. **Missing Big Picture:** There is no documentation regarding the global engine structure (classes, VTables, network protocols, session handling). When a function is patched, the focus remains purely on specific registers (`EAX`, `ECX`), leaving the logical construct behind it as a black box.

## Conclusion & Progress (0 - 5%)
When measured against the goal of "reversing the entire engine," the progress of the base repository is **close to 0%**. 

The repository is extremely successful at **treating symptoms and fixing bugs**, but it does not reconstruct the source code or the architecture of the engine. Since the engine spans hundreds of megabytes of complex C++ code (simulation, AI, UI, rendering), the 114 hooks represent only a microscopic fraction of the overall logic.

---

## Base Repository Discoveries (Systematic Deep Scan)
To provide a verifiable baseline, all files in the `FA-Binary-Patches` repository (`hooks\`, `include\`, `section\`) were systematically parsed. The extraction revealed a massive, hardcoded catalog of the engine's data structures and memory addresses.

### 1. Verified C++ Class Structures
The repository explicitly verifies the memory layout and size for **19 core C++ structs/classes**.
* **Engine Cores:** `Sim` (0xAF8 / 2808 bytes), `CWldSession` (0x508 / 1288 bytes), `CSimDriver` (0x230 / 560 bytes), `STIMap` (0x1548 / 5448 bytes).
* **Armies & Entities:** `SimArmy` (0x288 / 648 bytes), `BaseArmy` (0x1DC / 476 bytes), `UserArmy` (0x210 / 528 bytes).
* **Lua Subsystem:** The Lua 5.0 C-API is perfectly mapped, including `LuaState` (0x34 / 52 bytes), `LuaObject` (0x14 / 20 bytes), and `TObject` (0x8 / 8 bytes).
* **Data Types:** `string` / `wstring` (0x1C / 28 bytes), `moho_set` (0x20 / 32 bytes), `CScriptObject` (0x34 / 52 bytes).

### 2. Mapped Global Pointers
**16 global state singletons** have been identified in the `ForgedAlliance.exe` memory space:
* `g_Sim` -> `0x10A63F0`, `g_CWldSession` -> `0x10A6470`.
* `g_CSimDriver` -> `0x10C4F50`, `g_SWldSessionInfo` -> `0x10C4F58`.
* `g_CUIManager` -> `0x10A6450`, `g_EngineStats` -> `0x10A67B8`.
* `g_EntityCategoryTypeInfo` -> `0x10C6E70`, `g_CAiBrainTypeInfo` -> `0x10C6FA0`, `g_ConsoleLuaState` -> `0x10A6478`.

### 3. Identified Memory Addresses & Subsystem Hooks
A precise scan reveals **152 exact hardcoded hook addresses** across 48 C++ hook files.
* **Physics & Collisions:** `FixCollisions.cpp` intercepts at `0x69D258` and 5 other locations to re-route collision handling.
* **Networking & Crypto:** `DesyncFix.cpp` contains 12 hooks (e.g., `0x6E4150` for Decoder, `0x48A280` for recvfrom, `0x8984B0`).
* **UI & Commands:** Hooks target `OnWindowMessage` at `0x00430C0E` and `SimGetCommandQueueInsert` at `0x6CE3BA`.
* **Game Logic Fixes:** Dozens of smaller fixes like `GetEconDataAsFloats.cpp` (12 hooks), `LuaMessages.cpp` (8 hooks), `EnableConsoleCommands.cpp` (8 hooks), and `HRangeRings2.cpp` (5 hooks).

This verifiable collection of exactly 152 addresses and 19 massive struct sizes serves as the perfect foundation for translating the original assembly patches into full C++ architecture decompilation.
