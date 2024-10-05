void HookHydroCondition()
{
    asm(
        "sub     eax, 1;" // check if hydro
        "jnz 0x008701E1;" // not hydro, jump out
        // Check if we have any templates
        "mov     ecx, [esp+0x260];" // build_template.finish
        "sub     ecx, [esp+0x25C];" // build_template.start
        "mov     eax, 0x2E8BA2E9;"
        "imul    ecx;"
        "sar     edx, 3;"
        "mov     eax, edx;"
        "shr     eax, 0x1F;"
        "add     eax, edx;"
        // end
        "jnz 0x008701E1;"
        // snap hydro
        "mov ecx, [esp+0x1E4];" // comDat.blueprint
        "xor eax, eax;"
        "jmp 0x0087004D;"
        :
        :
        :);
}