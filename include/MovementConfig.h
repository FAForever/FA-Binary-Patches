#pragma once

// ==========================================================================
// MovementConfig.h — Engine offsets and shared helpers
// ==========================================================================

// Engine offsets (IDA-verified)
#define OFF_STEERING_UNIT       0x1C
#define OFF_UNIT_ID             0x70
#define OFF_UNIT_BLUEPRINT      0x74
#define OFF_UNIT_SIM            0x150
#define OFF_UNIT_POS            0x160
#define OFF_UNIT_MOTION         0x4B0
#define OFF_UNIT_NAVIGATOR      0x54C   // verified from NewMoveTask disasm (NOT 0x4B4)
#define OFF_SIM_CURTICK         0x900

// Blueprint economy
#define OFF_BP_MAXBUILDDIST     0x564   // float MaxBuildDistance

// Shared pointer validation
static inline bool IsValidPtr(uint32_t ptr) {
    return (ptr >= 0x00400000 && ptr < 0x3F000000 && (ptr & 3) == 0);
}
