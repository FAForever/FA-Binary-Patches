#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"

asm(
    SECTION(0, 0x0086FFF0)
    "NOP;"
    "NOP;"
    "NOP;"
    "NOP;"
    "NOP;"
    "NOP;"
    SECTION(1, 0x00870044)
    "jmp "QU(HookHydroCondition)";"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
);