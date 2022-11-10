asm(
  //energyConsumedAsFloat
  ".section h0; .set h0,0x8C52A5;"
  "movd ecx, xmm0;"
  ".section h1; .set h1,0x8C52B7;"
  "call 0x908320;"
  
  //massConsumedAsFloat
  ".section h2; .set h2,0x8C52DA;"
  "movd ecx, xmm0;"
  ".section h3; .set h3,0x8C52EC;"
  "call 0x908320;"
  
  // energyRequestedAsFloat
  ".section h4; .set h4,0x8C530F;"
  "movd ecx, xmm0;"
  ".section h5; .set h5,0x8C5321;"
  "call 0x908320;"
  
  // massRequestedAsFloat
  ".section h6; .set h6,0x8C5344;"
  "movd ecx, xmm0;"
  ".section h7; .set h7,0x8C5356;"
  "call 0x908320;"
  
  // energyProducedAsFloat
  ".section h8; .set h8,0x8C5379;"
  "movd ecx, xmm0;"
  ".section h9; .set h9,0x8C538B;"
  "call 0x908320;"
  
  // massProducedAsFloat
  ".section h10; .set h10,0x8C53B2;"
  "movd ecx, xmm0;"
  ".section h11; .set h11,0x8C53C0;"
  "call 0x908320;"
);