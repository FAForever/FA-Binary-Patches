
asm(
  ".section h0; .set h0,0x73D4A0;"
  "CALL "QU(HeightCylinders)";"

  ".section h1; .set h1,0x7EE2BC;"
  ".int "QU(MinCHeight)";"

  ".section h2; .set h2,0x7EE33A;"
  ".int "QU(MaxCHeight)";"

  ".section h3; .set h3,0x81C38E;"
  ".int "QU(MinCHeight)";"

  ".section h4; .set h4,0x81C3C5;"
  ".int "QU(MaxCHeight)";"
);