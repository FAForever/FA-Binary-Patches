asm(
    "LuaObject_SetNumber = 0x908320;"
    // energy consumed
    ".section h0; .set h0,0x8422FD;"
    "movd eax, xmm0;"
    ".section h1; .set h1,0x84230D;"
    "call LuaObject_SetNumber;"

     // mass consumed
    ".section h2; .set h2,0x842330;"
    "movd eax, xmm0;"
    ".section h3; .set h3,0x842340;"
    "call LuaObject_SetNumber;"

     // energy requested
    ".section h4; .set h4,0x842363;"
    "movd eax, xmm0;"
    ".section h5; .set h5,0x842373;"
    "call LuaObject_SetNumber;"

     // mass requested
    ".section h6; .set h6,0x842396;"
    "movd eax, xmm0;"
    ".section h7; .set h7,0x8423A6;"
    "call LuaObject_SetNumber;"

    // energy produced
    ".section h8; .set h8,0x8423C9;"
    "movd eax, xmm0;"
    ".section h9; .set h9,0x8423D9;"
    "call LuaObject_SetNumber;"

     // mass produced
    ".section h10; .set h10,0x842400;"
    "movd eax, xmm0;"
    ".section h11; .set h11,0x84240C;"
    "call LuaObject_SetNumber;"
);
