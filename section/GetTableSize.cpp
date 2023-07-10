#include "include/LuaAPI.h"

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
        "CALL 0x0090CD40;" // PushNumber
        "ADD ESP,0x8;"
        "MOV EAX,0x1;"
        "RET;"
        "Err:;"
        "XOR EAX,EAX;");
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
        "CALL 0x0090CF80;" // PushBool
        "ADD ESP,0x8;"
        "MOV EAX,0x1;");
}

void lua_createtable(/*lua_State *l, int narr, int nhash*/)
{
    asm( // copied from lua_newtable
        "push    esi;"
        "mov     esi, [esp+0x8];" // lua_State
        "mov     eax, [esi+0x10];"
        "mov     ecx, [eax+0x2C];"
        "cmp     ecx, [eax+0x24];"
        "mov  edx, [esp+0x0C];" // push narr
        "mov  ecx, [esp+0x10];" // push nhash
        "push    edi;"
        "jb      short loc_90D130;"
        "cmp     dword ptr [eax+0x28], 0;"
        "jnz     short loc_90D130;"
        "push    esi;"
        "call    0x915D90;"
        "add     esp, 4;"
        "loc_90D130: ;"
        "mov     edi, [esi+8];"
        "push    ecx;" // nhash
        "push    edx;" // narr
        "push    esi;"
        "call    0x00927320;" // luaH_new
        "movzx   edx, byte ptr [eax+4];"
        "mov     [edi], edx;"
        "mov     [edi+4], eax;"
        "mov     eax, [esi+8];"
        "mov     ecx, [esi+0x14];"
        "add     esp, 0x0C;"
        "cmp     eax, [ecx+4];"
        "jb      short loc_90D173;"
        "mov     edx, [esi+0x18];"
        "sub     edx, eax;"
        "mov     edi, 8;"
        "cmp     edx, edi;"
        "jg      short loc_90D16D;"
        "push    1;"
        "push    esi;"
        "call    0x913990;"
        "add     esp, 8;"
        "loc_90D16D:;"
        "add     [esi+8], edi;"
        "pop     edi;"
        "pop     esi;"
        "ret;"
        "loc_90D173:;"
        "add     dword ptr [esi+8], 8;"
        "pop     edi;"
        "pop     esi;"
        "ret;");
}

int _CreateTable(lua_State *l)
{
    int narr = luaL_checknumber(l, 1);
    int nhash = 0;
    if (lua_isnumber(l, 2))
    {
        nhash = lua_tonumber(l, 2);
    }
    reinterpret_cast<void (*)(lua_State *, int, int)>(&lua_createtable)(l,
                                                                        narr > 0 ? narr : 0,
                                                                        nhash > 0 ? nhash : 0);
    return 1;
}

int RegTableFuncsDesc[] = {
    "getsize2", &GetTableSize,
    "empty2", &IsTableEmpty,
    "getn2", 0x00927C20,
    "create", &_CreateTable,
    0, 0};

void RegTableFuncs()
{
    asm(
        "CALL 0x0090DE00;"
        "MOV DWORD PTR [ESP+0x8],%[RegTableFuncsDesc];"
        "MOV DWORD PTR [ESP+0xC],0x0;"
        "CALL 0x0090DE00;"
        "JMP 0x009283B6;"
        :
        : [RegTableFuncsDesc] "i"(RegTableFuncsDesc)
        :);
}