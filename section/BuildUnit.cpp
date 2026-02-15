char _sCQUEMOV[] = "CQUEMOV";
char** _pCQUEMOV = (char**)&_sCQUEMOV;

void BuildUnit()
{
    asm(
        "PUSH 0x00E19824;"
        "JMP BU_L1;"
        "BU_L2:;"
    );
    asm(
        "PUSH %[_pCQUEMOV];"
        "MOV EDX,SS:[ESP-0x80];"
        "MOV ECX,SS:[ESP];"
        "MOV EDX,SS:[EDX];"
        "MOV ECX,SS:[ECX];"
        "CMP EDX,ECX;"
        "JE 0x006EFACE;"
        "MOV SS:[ESP+0x20],EAX;"
        "BU_L1:;"
        "LEA ECX,[ESP+0x44];"
        "CALL 0x00405550;"
        "MOV BYTE PTR SS:[ESP+0x36C],1;"
        "MOV EBX,1;"
        "LEA ECX,[EDI+8];"
        "LEA EAX,[ESP+0x40];"
        "MOV SS:[ESP+0x18],EBX;"
        "CALL 0x0067B050;"
        "TEST AL,AL;"
        "JNE 0x006EFAF8;"
        "JMP BU_L2;"
        :
        : [_pCQUEMOV] "d" (_pCQUEMOV)
        :
    );
}