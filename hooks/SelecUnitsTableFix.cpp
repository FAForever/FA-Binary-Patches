#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"

asm(
    // nop AssignNewTable call
    SECTION(0, 0x008BD9BF)
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    SECTION(1, 0x008BD9CE)
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    // nop PushStack
    SECTION(2, 0x008BDBA6)
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"

    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"

    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    // return 0 value on stack
    SECTION(3, 0x008BDC18)
    "mov eax, 0;"
);