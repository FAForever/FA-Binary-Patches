#include "global.h"
#include "magic_classes.h"

float reclaimCameraDistance asm("reclaimCameraDistance") = 150.0f;

// Loads the reclaim distance selected by UI Lua. Lua restores the vanilla
// distance of 150 whenever explicit reclaim command mode is not active.
extern "C" __attribute__((naked)) void LoadReclaimCameraDistance()
{
    asm volatile(
        "FLD DWORD PTR [reclaimCameraDistance];"
        "RET;"
    );
}

ConDescReg<float> ReclaimCameraDistanceConsole{
    "ui_ReclaimCameraDistance",
    "Camera distance for issuing reclaim orders",
    &reclaimCameraDistance};
