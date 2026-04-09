extern "C" void __attribute__((naked)) asm__ShouldAddUnit() {
  asm volatile(R"(
    push ecx;
    push edx;
    push edi;
    push eax;
    call _ShouldAddUnit;
    add esp, 8;
    pop edx;
    pop ecx;
    test al,al;
    je 0x007A789F;
    jmp 0x007A7860;)");
}
