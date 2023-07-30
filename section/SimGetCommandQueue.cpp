#include "include/moho.h"
#include "include/LuaAPI.h"

void __thiscall SimGetCommandQueueInsert(LuaObject *this_, LuaObject *obj)
{
    auto unitCmd = reinterpret_cast<uintptr_t>(obj) - 0x20;
    obj->SetInteger("commandType", *reinterpret_cast<int*>(unitCmd + 0x98));
    obj->SetNumber("x", *reinterpret_cast<float*>(unitCmd + 0xA4));
    obj->SetNumber("y", *reinterpret_cast<float*>(unitCmd + 0xA8));
    obj->SetNumber("z", *reinterpret_cast<float*>(unitCmd + 0xAC));
    auto targetId = *reinterpret_cast<unsigned long*>(unitCmd + 0xA0);
    if (targetId != 0xF0000000) {
        char buf[16];
        sprintf_s(buf, sizeof(buf), "%d", targetId);  //like game doing entityId with std::string
        obj->SetString("targetId", buf);
    }
    auto v3 = *reinterpret_cast<uintptr_t*>(unitCmd + 0x60);
    if (v3) //like in sub_842770
    {
        const char* bpId;
        if ( *(int *)(v3 + 32) < 0x10u )
            bpId = (const char *)(v3 + 12);
        else
            bpId = *(const char **)(v3 + 12);
        obj->SetString("blueprintId", bpId);
    }
    this_->Insert(obj);
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
