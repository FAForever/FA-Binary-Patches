#include "moho.h"

void __thiscall SimGetCommandQueueInsert(LuaObject *this_, LuaObject *obj)
{
    auto command = reinterpret_cast<incomplete::Command*>(reinterpret_cast<uintptr_t>(obj) + 0x20);
    obj->SetInteger("commandType", command->commandType);
    obj->SetNumber("x", command->pos.x);
    obj->SetNumber("y", command->pos.y);
    obj->SetNumber("z", command->pos.z);
    auto targetId = command->targetId;
    if (targetId != 0xF0000000) {
        char buf[16];
        sprintf_s(buf, sizeof(buf), "%d", targetId);  //like game doing entityId with std::string
        obj->SetString("targetId", buf);
    }
    auto bp = command->bpBuild;
    if (bp)
        obj->SetString("blueprintId", bp->name.data());
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
