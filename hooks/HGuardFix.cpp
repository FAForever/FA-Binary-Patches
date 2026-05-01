
asm(
  ".section h0; .set h0,0x6ACC95;"
  "JMP "QU(GuardZero)";"
  "NOP;"
  "NOP;"
  "NOP;"
);
