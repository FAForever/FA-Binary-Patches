
asm(
  ".section h1; .set h1,0x45A926;"
  "push eax;"
  "call InitCtors;"
  "pop eax;"
  "ret 8;"
);