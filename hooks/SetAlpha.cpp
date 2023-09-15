#include "../define.h"
asm(
    //prevent set alpha from resetting color mask
    ".section h0; .set h0,0x789988;"
    "call "QU(_SetAlpha)";"

    //check for bool not type != 0
    ".section h1; .set h1,0x789972;"
    "call 0x90CA40;" //call lua_toboolean

    //apply to children
    ".section h2; .set h2,0x789999;"
    "sub esp, 4;"
    "fstp dword ptr [esp];"
    "call "QU(_SetAlphaChildren)";"
    "jmp short 0x7899FB;"
);