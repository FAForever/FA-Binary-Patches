
asm(
  ".section h0; .set h0,0x61FF7B;"
  "JMP StopReclaim;"
  "nop;"
  "nop;"
  
  ".section h1; .set h1,0x620071;"
  "JMP RestoreReclaimValues;"
  "nop;"
  "nop;"
);