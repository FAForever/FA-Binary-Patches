int Colors[32];

// Basically what we do here is parsing colors from string in lua function TeamColorMode() (originally it supports only bool)
// then store them in array and then use it instead of one created from GameColors.lua
// the string should contains colors in hex format like that: "ffffffff,ffffffff,ffffffff,ffffffff,ffffffff"
// colors separated by commas, no spaces
// then stored colors will be applied to armies according to their indexes.

void TeamColorMode()
{
    asm(
        "mov dword ptr [esp+0xC], 0x1;"
        "push 1;"
        "push ecx;"
        "call 0x90C740;" //lua_type
        "add esp,0x8;"
        "cmp eax, 0x1;"
        "je 0x847E59;" //Return if bool
        "push 1;"
        "push ecx;"
        "call 0x90CA90;" //lua_tostring
        "add esp,0x8;"

        // Parse hex colors from string and store them to array
        "mov esi, %[Colors];"
        "xor ecx, ecx;"

        "Start:;"
        "cmp byte ptr [eax+ecx], 0;"
        "je 0x847E64;" //Return if string done
        "xor edx, edx;"

        "HexStart:;"
        "movsx edi, BYTE PTR [eax+ecx];"
        
        //Digit
        "lea ebx, [edi-48];"
        "cmp ebx, 9;"
        "jbe Sym;"

        //Upper
        "lea ebx, [edi-65];"
        "cmp ebx, 5;"
        "ja Lower;"
        "lea ebx, [edi-55];"
        "jmp Sym;"

        "Lower:;"
        "lea ebx, [edi-97];"
        "cmp ebx, 5;"
        "ja Store;"
        "lea ebx, [edi-87];"

        "Sym:;"
        "sal edx, 4;"
        "inc ecx;"
        "add edx, ebx;"
        "jmp HexStart;"

        "Store:;"
        "mov DWORD PTR [esi], edx;"
        "inc ecx;"
        "add esi, 4;"
        "jmp Start;"
        :
        : [Colors] "i" (Colors)
        :
    );
}

void TeamColorModeRenderer()
{
    asm(
        "push eax;"
        "mov ecx, dword ptr [edx];"
        "mov eax, %[Colors];"
        "mov ecx, dword ptr [eax+ecx*0x4];"
        "pop eax;"
        "jmp 0x85DB77;"
        :
        : [Colors] "i" (Colors)
        :
    );
}
