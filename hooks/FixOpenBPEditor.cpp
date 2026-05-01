
asm(
  //SC_CreateEntityDialog
  ".section h0; .set h0,0x8D4010;"
  "JMP "QU(FixCreateEntityDialog)";"
  "NOP;"
);