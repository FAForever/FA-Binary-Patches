
asm(
  //HOOK BuildUnit
  ".section h0; .set h0,0x6EFAC9;"
  "JMP "QU(BuildUnit)";"
  "ADD ESP,4;"
  "xor ecx,ecx;"
  "xor eax,eax;"
  "jmp 0x6EFB0E;" //_CannotQueCommandInConstruct
  "nop;"
  "nop;"
  "nop;"
  "nop;"
  "nop;"

  //HOOK SelectUnit
  ".section h1; .set h1,0x8C05FE;"
  "JMP "QU(SelectUnit)";"

  //HOOK SimArmyCreate
  ".section h2; .set h2,0x6FF3D1;"
  "JMP "QU(SimArmyCreate)";"
  "NOP;"
  "call 0x4059E0;"
  "mov bl,0x28;"
  "lea ecx, [esp+0x10C];"
  "mov byte [esp+0x150],bl;"
  "call 0x9075D0;"
  "mov ecx, dword ptr [esp+0x7C];"
  "nop;"
  "nop;"
  "push 0x3;"
  "push 0xE2F868;" //s_Off
  "push ecx;"
  "push edi;"
  "lea ecx, [esp+0x78];"
  "call 0x40A880;"
  "test eax,eax;"
  "je TimerIsOff;"
  "jmp  0x6FF47D;" //_EndTimerValidation
  "TimerIsOff:;"
  "mov dword ptr [ebp+0x1CC],0x0;"
  "jmp  0x6FF47D;" //_EndTimerValidation
  "nop;"
  "nop;"
);
