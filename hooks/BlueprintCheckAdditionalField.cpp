#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x004CF7F2)
    "jmp "QU(GetBpFieldValues)";"
    
    SECTION(2, 0x004CF7F7)
    "jmp "QU(ProcessBpFieldValues)";"
    "nop;"
    
    SECTION(3, 0x005283D5)
    "jmp "QU(InitRMeshBlueprint)";"
    "nop;"
);