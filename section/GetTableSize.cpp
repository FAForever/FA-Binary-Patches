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
        "JZ GTS_L2;"
        "MOV EDX,1;"
        "SHL EDX,CL;"
        "MOV ECX,[EAX+0x14];"
        "GTS_L3:;"
        "CMP DWORD PTR [ECX+0x8],0x0;"
        "JE GTS_L4;"
        "ADD EBX,0x1;"
        "GTS_L4:;"
        "ADD ECX,0x14;"
        "DEC EDX;"
        "JNZ GTS_L3;"
        "GTS_L2:;"
        "MOV EDX,[EAX+0x20];"
        "TEST EDX,EDX;"
        "JZ GTS_L7;"
        "MOV ECX,[EAX+0x10];"
        "GTS_L5:;"
        "CMP DWORD PTR [ECX],0x0;"
        "JE GTS_L6;"
        "ADD EBX,0x1;"
        "GTS_L6:;"
        "ADD ECX,0x8;"
        "DEC EDX;"
        "JNZ GTS_L5;"
        "GTS_L7:;"
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
        "JAE ITE_L72;"
        "CMP DWORD PTR [EAX],0x5;"
        "JNE ITE_L72;"
        "MOV EAX,[EAX+0x4];"
        "MOV CL,[EAX+0x9];"
        "TEST CL,CL;"
        "JZ ITE_L22;"
        "MOV EDX,1;"
        "SHL EDX,CL;"
        "MOV ECX,[EAX+0x14];"
        "ITE_L32:;"
        "CMP DWORD PTR [ECX+0x8],0x0;"
        "JNE ITE_L62;"
        "ADD ECX,0x14;"
        "DEC EDX;"
        "JNZ ITE_L32;"
        "ITE_L22:;"
        "MOV EDX,[EAX+0x20];"
        "TEST EDX,EDX;"
        "JZ ITE_L72;"
        "MOV ECX,[EAX+0x10];"
        "ITE_L5:;"
        "CMP DWORD PTR [ECX],0x0;"
        "JNE ITE_L62;"
        "ADD ECX,0x8;"
        "DEC EDX;"
        "JNZ ITE_L5;"
        "ITE_L72:;"
        "PUSH 0x1;"
        "JMP ITE_L12;"
        "ITE_L62:;"
        "PUSH 0x0;"
        "ITE_L12:;"
        "PUSH ESI;"
        "CALL 0x0090CF80;"     //PushBool
        "ADD ESP,0x8;"
        "MOV EAX,0x1;"
    );
}

#include "include/LuaAPI.h"
int TableClone(lua_State* L)
{
    LuaObject obj{L->LuaState, 1};
    LuaObject cloned{};
    obj.Clone(&cloned);
    cloned.PushStack(L);
    return 1;
}

int RegTableFuncsDesc[] = {"getsize2",&GetTableSize,
                           "empty2",&IsTableEmpty,
                           "getn2",0x00927C20,
                           "clone", &TableClone,
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