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

// CAiNavigatorLand (IDA-verified from SetGoal disasm)
#define OFF_NAVLAND_PATHNAV     0x68    // CAiPathNavigator*

// CAiPathNavigator (IDA-verified from UpdateCurrentPosition disasm)
#define OFF_PATHNAV_NEXT        0x0C    // mNext (0 = arrived)
#define OFF_PATHNAV_CURPOS      0x24    // mCurrentPos (HPathCell: x=int16 low, z=int16 high)
#define OFF_PATHNAV_TARGETPOS   0x28    // mTargetPos
#define OFF_PATHNAV_GOAL_X      0x30    // mGoal.mPos1.x0 (set by SetGoal)
#define OFF_PATHNAV_GOAL_Z      0x34    // mGoal.mPos1.z0
#define OFF_PATHNAV_TAG_X       0x40    // mGoal.mPos2.x0 — build center X (our tag)
#define OFF_PATHNAV_TAG_Z       0x44    // mGoal.mPos2.z0 — build center Z (our tag)
#define OFF_PATHNAV_TAG_RANGE   0x48    // mGoal.mPos2.x1 — rangeCells (0=inactive)
#define OFF_PATHNAV_V26         0x64    // v26 counter (set to 0 on arrival)

// Shared pointer validation
static inline bool IsValidPtr(uint32_t ptr) {
    return (ptr >= 0x00400000 && ptr < 0x3F000000 && (ptr & 3) == 0);
}
