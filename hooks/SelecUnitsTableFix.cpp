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
    "mov byte ptr [esp+0xD0-0xC], 2;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    // nop PushStack
    SECTION(1, 0x008BDBA6)
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
    SECTION(2, 0x008BDC18)
    "mov eax, 0;"
);