void SimArmyCreate()
{
    asm(
        "SUB ECX,ESI;"
        "PUSH ECX;"
        "PUSH EAX;"
        "PUSHAD;"
        "MOV ESI,0;"
        "MOV EBX,0;"
"SAC_L1: PUSH EAX;"
        "MOV EAX,0x0A;"
        "MUL BX;"
        "MOV EBX,EAX;"
        "POP EAX;"
        "MOV EDX,DS:[ESI+EAX];"
        "SUB EDX,0x30;"
        "ADD BL,DL;"
        "INC ESI;"
        "LOOP SAC_L1;"
        "MOV EAX,0x258;"
        "MUL BX;"
        "MOV SS:[EBP+0x1CC],EAX;"
        "POPAD;"
        "LEA ECX,[ESP+0x70];"
        "JMP 0x6FF3D6;"
    );
}