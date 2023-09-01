void IssueBuildMobileFix()
{
    asm(
        "push    5;"
        "push    esi;"
        "call    0x90CA40;" // lua_toboolean
        "add esp, 8;"
        "test al, al;"
        "jz PICK_NEAREST;"
        "lea edx, [esp+0x94];"
        "jmp UNIT_ISSUE;"
        "PICK_NEAREST:"
        "lea edx, [esp+0x6C];"
        "UNIT_ISSUE:"
        "call    0x6F12C0;"
        "jmp     0x6F5FA0;");
}
