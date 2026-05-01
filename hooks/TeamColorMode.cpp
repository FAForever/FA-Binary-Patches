
asm(
  //HOOK TeamColor
  ".section h0; .set h0,0x847E51;"
  "JMP "QU(TeamColorMode)";"
  "nop;"
  "nop;"
  "nop;"

  //HOOK TeamColorRenderer
  ".section h1; .set h1,0x85DB68;"
  "JMP "QU(TeamColorModeRenderer)";"
  "nop;"
  "nop;"
);