#include "include/moho.h"
#include "include/LuaAPI.h"

void __thiscall SimGetCommandQueueInsert(LuaObject *this_, LuaObject *obj)
{
    auto unit = reinterpret_cast<uintptr_t>(obj) - 0x20;
    CLuaObject::SetInteger(obj, "commandType", *reinterpret_cast<int*>(unit + 0x98));
    CLuaObject::SetNumber(obj, "x", *reinterpret_cast<float*>(unit + 0xA4));
    CLuaObject::SetNumber(obj, "y", *reinterpret_cast<float*>(unit + 0xA8));
    CLuaObject::SetNumber(obj, "z", *reinterpret_cast<float*>(unit + 0xAC));
    auto targetId = *reinterpret_cast<unsigned long*>(unit + 0xA0);
    if (targetId != 0xF0000000) {
        char buf[16];
        sprintf_s(buf, sizeof(buf), "%d", targetId);  //like game doing entityId with std::string
        CLuaObject::SetString(obj, "targetId", buf);
    }
    auto v3 = *reinterpret_cast<uintptr_t*>(unit + 0x60);
    if (v3) //like in sub_842770
    {
        const char* bpId;
        if ( *(int *)(v3 + 32) < 0x10u ) 
            bpId = (const char *)(v3 + 12);
        else
            bpId = *(const char **)(v3 + 12);
        CLuaObject::SetString(obj, "blueprintId", bpId);
    }
    CLuaObject::Insert(this_, obj);
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
