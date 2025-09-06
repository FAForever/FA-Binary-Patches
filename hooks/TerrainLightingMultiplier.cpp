#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x0080163F)
    "jmp "QU(GetTerrainLighMult)";"
    "nop;"
    "nop;"
    "nop;"
    
    SECTION(2, 0x008016D9)
    "jmp "QU(GetSunColor)";"
    "nop;"
    
    SECTION(3, 0x00801673)
    "jmp "QU(GetSunPosition)";"
    "nop;"
);