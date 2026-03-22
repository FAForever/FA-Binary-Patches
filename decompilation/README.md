# Decompilation Documentation

This directory contains decompiled C++ code and architectural findings of `ForgedAlliance.exe` (v3599), `MohoEngine.dll`, and related game binaries.

## Methodology

The decompilation process utilizes **Ghidra** (with MCP server integration) and AI-assisted analysis to extract and verify internal structures, vtable layouts, function signatures, and field offsets.

> **Disclaimer:** All decompiled code, signatures, and field offsets must be manually reviewed and verified. AI-inferred logic may contain inaccuracies – always cross-check with live Ghidra disassembly.

## "Engine as Library" Approach

Rather than attempting a full source reconstruction (infeasible for a ~2M LOC C++ binary), the goal is to expose the engine's internal systems as clean, callable C++ via:

- **`include/engine.h`** – Master header: full structs, vtable wrappers, and inline functions for all major subsystems (CWldSession, Sim, CSimDriver, CLobby, IClientManager, and all WLD/NET/UI/Console/GpgNet APIs)
- **`include/moho.h`** – C++ struct definitions with verified field offsets
- **`include/global.h`** – Global pointer accessors and memory-mapped function declarations

### Writing a New Hook with `engine.h`

```cpp
#include "../include/engine.h"

// Access engine globals directly:
CWldSession* session = g_CWldSession;
Sim*         sim     = g_Sim;

// Call engine methods cleanly:
int beat       = sim->beatCounter;
bool isMP      = session->isMultiplayer;
bool isPaused  = session->IsPaused();
uint armyCount = sim->GetArmyCount();

// Call DLL functions:
WLD_RequestEndSession();
CON_Execute("sc_toggleconsole");
UI_NoteGameOver();
```

See `hooks/ExampleEngineHook.cpp` for a complete working example.

## Files

| File | Description |
|------|-------------|
| `ForgedAlliance.exe.md` | Named functions & global pointers (200+ entries) |
| `MohoEngine.dll.md` | DLL export analysis |
| `engine-internals.md` | Detailed subsystem documentation (25+ systems) |
| `use-cases.md` | Planned features & how engine internals enable them |
| `docs/` | Debugging guides and progress reports |

## Progress

| Subsystem | Status | Source |
|-----------|--------|--------|
| CWldSession | ✅ Full struct (0x510 bytes), all key methods | `engine.h` |
| Sim | ✅ Full struct, singleton API, beat/tick/checksum | `engine.h` |
| CSimDriver | ✅ Core struct, vtable wrappers | `engine.h` |
| CLobby | ✅ Core struct, peer management, SPeer states | `engine.h` |
| IClientManager | ✅ GetSimRate/SetSimRate/CalcMaxSimRate | `engine.h` |
| WLD Session API | ✅ All exports wrapped as inline functions | `engine.h` |
| NET API | ✅ MakeConnector, TCP, UDP exports | `engine.h` |
| UI API | ✅ StartGame, FrontEnd, Desync, GameOver | `engine.h` |
| Console API | ✅ Execute, Executef, Printf | `engine.h` |
| Replay/Observer | ✅ IsReplay, BecomeObserver, VCR exports | `engine.h` |
| GpgNet API | ✅ ReportDesync, SubmitArmyStats | `engine.h` |
| Threading | ✅ IsMainThread, InvokeAsync/Wait | `engine.h` |
| Entity/Unit | 🔄 Partial (moho.h) | `moho.h` |
| AI (CAiBrain) | 🔄 Partial | `moho.h` |
| Pathfinding | ❌ Not started | - |
| Weapon/Damage | ❌ Not started | - |
| Save/Load | ❌ Not started | - |

**Overall RE Progress:** ~5%
