#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x006A56FE)
    "jmp "QU(UnitInit)";"
    "nop;"
    
    
    SECTION(2, 0x005B34FB)
    "jmp "QU(UnitForceReverseCheck)";"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    
    SECTION(3, 0x005B3578)
    "jmp "QU(BackUpDistanceCheck)";"
);