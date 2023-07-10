#include "include/moho.h"
#include "include/LuaAPI.h"

int commandType;
int targetId;
char* bpId;
float x;
float y;
float z;

LuaState* LuaS;
LuaObject* O;
LuaStackObject* S;
LuaObject SubTable;

void SimGetCommandQueueCpp()
{
    CLuaObject::AssignNewTable(&SubTable, LuaS, 0, 0);
    CLuaObject::SetNumber(&SubTable, "commandType", commandType);
    CLuaObject::SetNumber(&SubTable, "x", x);
    CLuaObject::SetNumber(&SubTable, "y", y);
    CLuaObject::SetNumber(&SubTable, "z", z);
    
    if (targetId != -1)
    {
        CLuaObject::SetInteger(&SubTable, "targetId", targetId);
    }
    
    CLuaObject::SetString(&SubTable, "blueprintId", bpId);
    CLuaObject::Insert(O, SubTable);
    CLuaObject::DLuaObject(&SubTable);
}

void SimGetCommandQueue()
{
	asm(
        "mov %[LuaS], ebx;"         //LuaState
        "lea ebx, [esp+0x20];"
        "mov %[O], ebx;"            //Main table
        "lea ebx, [esp+0x18];"
        "mov %[S], ebx;"            //Stack
        "push 0;"
        "push 0;"
        "push 0;"
        "push 0;"
        "lea ebx, [esp];"
        "mov %[SubTable], ebx;"     //SubTable
        "mov ebx, %[LuaS];"
        
        
        "push edx;"
        "push eax;"
        "mov eax, [esi];"
        "mov edx, [eax+0x94];"
        "mov %[commandType], edx;"  //commandType

        "mov edx, [eax+0xA0];"
        "mov %[x], edx;"            //X
        
        "mov edx, [eax+0xA4];"
        "mov %[y], edx;"            //Y
        
        "mov edx, [eax+0xA8];"      
        "mov %[z], edx;"            //Z
        
        "mov %[targetId], -1;"
        "mov edx, [eax+0x9C];"
        "cmp edx, 0xF0000000;"
        "je Blueprint;"
        "mov %[targetId], edx;"     //targetId  
        
        "Blueprint:;"
        "mov %[bpId], 0x0;"
        "mov edx, [eax+0x5C];"
        "cmp edx, 0x0;"
        "je End;"
        "add edx, 0xC;"
        "mov %[bpId], edx;"         //blueprintId
        
        "End:;"
        "pop eax;"
        "pop edx;"
        "jmp 0x6CE3B7;"
        :
        : [commandType] "m" (commandType), [targetId] "m" (targetId), [bpId] "m" (bpId), [x] "m" (x), [y] "m" (y), [z] "m" (z), [SubTable] "m" (SubTable), [LuaS] "m" (LuaS), [O] "m" (O), [S] "m" (S)
        :
	);
}

/* Commands
1 = Stop
2 = Move
3 = Dive
4 = FormMove
5 = BuildSiloTactical
6 = BuildSiloNuke
7 = BuildFactory
8 = BuildMobile
9 = BuildAssist
10 = Attack
11 = FormAttack
12 = Nuke
13 = Tactical
14 = Teleport
15 = Guard
16 = Patrol
17 = Ferry
18 = FormPatrol
19 = Reclaim
20 = Repair
21 = Capture
22 = TransportLoadUnits
23 = TransportReverseLoadUnits
24 = TransportUnloadUnits
25 = TransportUnloadSpecificUnits
26 = DetachFromTransport
27 = Upgrade
28 = Script
29 = AssistCommander
30 = KillSelf
31 = DestroySelf
32 = Sacrifice
33 = Pause
34 = OverCharge
35 = AggressiveMove
36 = FormAggressiveMove
37 = AssistMove
38 = SpecialAction
39 = Dock
*/