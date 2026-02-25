asm(R"(
  .section h0; .set h0,0x007A7858;
  jmp asm__ShouldAddUnit;
  nop;

  .section h1; .set h1,0x00E4D984;
  .int Moho__UserUnit__GetIntelRanges;
)");
