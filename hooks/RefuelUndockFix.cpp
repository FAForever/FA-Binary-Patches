asm(
    // CUnitRefuel::TaskTick, carrier TASKSTATE_Waiting (mIsCarrier, state 1):
    // relax health == maxHealth to health >= maxHealth
    ".section h0; .set h0,0x6217F8;"
    "jb 0x62174E;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"

    // CUnitRefuel::TaskTick, air staging TASKSTATE_Processing (state 4):
    // relax health == maxHealth to health >= maxHealth
    ".section h1; .set h1,0x621DF1;"
    "jb 0x621E67;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
);
