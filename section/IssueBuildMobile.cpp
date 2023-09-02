void IssueBuildMobileFix()
{
    asm(
        "push    eax;"
        "push    5;"
        "push    esi;"
        "call    0x90CA40;" // lua_toboolean
        "add esp, 8;"
        "test al, al;"
        "jz PICK_NEAREST;"
        "lea edx, [esp+0x94];"
        "jmp    0x6F5F9B;"//UNIT_ISSUE
        "PICK_NEAREST:"
        "lea edx, [esp+0x6C];"
        "jmp     0x6F5F9B;");
}
