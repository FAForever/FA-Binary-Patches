
asm(
    ".section h0; .set h0,0x86EF2B;"
    "jmp CustomDrawEnter;"

    //allow changing thickness of line
    ".section h1; .set h1,0x456264;"
    "subss xmm0, ds:THICKNESS;"

    //allow changing thickness of line
    ".section h2; .set h2,0x4562CD;"
    "addss xmm0, ds:THICKNESS;"

    //allow changing color
    ".section h3; .set h3,0x4561A8;"
    "jmp _SetColor;"

    //increase number of segements
    ".section h4; .set h4,0x455E53;"
    "mov ecx, 0x30;"
    
    ".section h5; .set h5,0x4561E4;"
    "cmp eax, 0x30;"
    //adjust angle to number of segments 2*pi/segments
    ".section h6; .set h6,0x455E30;"
    "fld dword ptr [SEGMENT_RAD];"
);
