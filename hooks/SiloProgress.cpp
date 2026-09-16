#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"

asm(
    
    SECTION(1, 0x005CF42A)
    "lea eax, [esp+0x18];"
    "push eax;"
    "push edi;"
    "call CheckSiloProgress;"
    "jmp 0x005CF4CA;"
    
    SECTION(2, 0x005CF030)
    "jmp AddSiloEco;"
    
);