char sCQUEMOV[] = "CQUEMOV";
char** pCQUEMOV = (char**)&sCQUEMOV;

void SelectUnit()
{
    asm(
        "PUSH %[pCQUEMOV];"
        "LEA ECX,[ESP+0x50];"
        "CALL 0x00405550;"
        "MOV DWORD PTR SS:[ESP+0x70],1;"
        "OR EBX,00000002;"
        "LEA EAX,[ESP+0x4C];"
        "MOV ECX,ESI;"
        "MOV SS:[ESP+0x10],EBX;"
        "CALL 0x008B97C0;"
        "TEST AL,AL;"
        "JNZ 0x008C062A;"
        "PUSH 0x00E19824;"
        "JMP 0x008C0603;"
        :
        : [pCQUEMOV] "c" (pCQUEMOV)
        :
    );
}