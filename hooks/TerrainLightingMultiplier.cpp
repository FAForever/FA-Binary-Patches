#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x0080163F)
    "jmp "QU(GetTerrainLighMult)";"
    "nop;"
    "nop;"
    "nop;"
);