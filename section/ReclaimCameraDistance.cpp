#include "global.h"
#include "magic_classes.h"

bool explicitReclaimMode asm("explicitReclaimMode") = false;
float explicitReclaimCameraDistance asm("explicitReclaimCameraDistance") = 150.0f;

// Returns the extended distance only while the UI has explicitly entered the
// reclaim command mode. Contextual right-click reclaim continues to read the
// engine's original distance (150) from 0x00E4F988.
extern "C" __attribute__((naked)) void LoadReclaimCameraDistance()
{
    asm volatile(
        "CMP BYTE PTR [explicitReclaimMode],0;"
        "JE 1f;"
        "FLD DWORD PTR [explicitReclaimCameraDistance];"
        "RET;"
        "1:;"
        "FLD DWORD PTR [0x00E4F988];"
        "RET;"
    );
}

ConDescReg<bool> ExplicitReclaimModeConsole{
    "ui_ExplicitReclaimMode",
    "Use ui_ExplicitReclaimCameraDistance while explicit reclaim command mode is active",
    &explicitReclaimMode};

ConDescReg<float> ExplicitReclaimCameraDistanceConsole{
    "ui_ExplicitReclaimCameraDistance",
    "Camera distance for issuing explicit reclaim orders",
    &explicitReclaimCameraDistance};
