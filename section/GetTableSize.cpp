void GetTableSize()
{
    asm(
        "MOV EAX,[ESI+0xC];"
        "CMP EAX,[ESI+0x8];"
        "JAE Err;"
        "CMP DWORD PTR [EAX],0x5;"
        "JNE Err;"
        "MOV EAX,[EAX+0x4];"
        "XOR EBX,EBX;"
        "MOV CL,[EAX+0x9];"
        "TEST CL,CL;"
        "JZ L2;"
        "MOV EDX,1;"
        "SHL EDX,CL;"
        "MOV ECX,[EAX+0x14];"
        "L3:;"
        "CMP DWORD PTR [ECX+0x8],0x0;"
        "JE L4;"
        "ADD EBX,0x1;"
        "L4:;"
        "ADD ECX,0x14;"
        "DEC EDX;"
        "JNZ L3;"
        "L2:;"
        "MOV EDX,[EAX+0x20];"
        "TEST EDX,EDX;"
        "JZ L7;"
        "MOV ECX,[EAX+0x10];"
        "L5:;"
        "CMP DWORD PTR [ECX],0x0;"
        "JE L6;"
        "ADD EBX,0x1;"
        "L6:;"
        "ADD ECX,0x8;"
        "DEC EDX;"
        "JNZ L5;"
        "L7:;"
        "CVTSI2SS XMM0,EBX;"
        "SUB ESP,0x4;"
        "MOVSS [ESP],XMM0;"
        "PUSH ESI;"
        "CALL 0x0090CD40;"     //PushNumber
        "ADD ESP,0x8;"
        "MOV EAX,0x1;"
        "RET;"
        "Err:;"
        "XOR EAX,EAX;"
    );
}

void IsTableEmpty()
{
    asm(
        "MOV EAX,[ESI+0xC];"
        "CMP EAX,[ESI+0x8];"
        "JAE L072;"
        "CMP DWORD PTR [EAX],0x5;"
        "JNE L072;"
        "MOV EAX,[EAX+0x4];"
        "MOV CL,[EAX+0x9];"
        "TEST CL,CL;"
        "JZ L22;"
        "MOV EDX,1;"
        "SHL EDX,CL;"
        "MOV ECX,[EAX+0x14];"
        "L32:;"
        "CMP DWORD PTR [ECX+0x8],0x0;"
        "JNE L62;"
        "ADD ECX,0x14;"
        "DEC EDX;"
        "JNZ L32;"
        "L22:;"
        "MOV EDX,[EAX+0x20];"
        "TEST EDX,EDX;"
        "JZ L072;"
        "MOV ECX,[EAX+0x10];"
        "ITE_L5:;"
        "CMP DWORD PTR [ECX],0x0;"
        "JNE L62;"
        "ADD ECX,0x8;"
        "DEC EDX;"
        "JNZ ITE_L5;"
        "L072:;"
        "PUSH 0x1;"
        "JMP L12;"
        "L62:;"
        "PUSH 0x0;"
        "L12:;"
        "PUSH ESI;"
        "CALL 0x0090CF80;"     //PushBool
        "ADD ESP,0x8;"
        "MOV EAX,0x1;"
    );
}

int RegTableFuncsDesc[] = {"getsize2",&GetTableSize,
                           "empty2",&IsTableEmpty,
                           "getn2",0x00927C20,
                           0,0};

void RegTableFuncs()
{
    asm(
        "CALL 0x0090DE00;"
        "MOV DWORD PTR [ESP+0x8],%[RegTableFuncsDesc];"
        "MOV DWORD PTR [ESP+0xC],0x0;"
        "CALL 0x0090DE00;"
        "JMP 0x009283B6;"
        :
        : [RegTableFuncsDesc] "i" (RegTableFuncsDesc)
        :
    );
}