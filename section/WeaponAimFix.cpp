// Nomander's comment from Github explaining what are we doing here:
//
// "After .text:00631A6D the aim manipulator aiming calculation adds the target's velocity to the target point. 
// The remainder of the calculations is correctly calculating ballistics, but this specific part needs to check if 
// the target's position was already updated since it's part of the entity task tick stage, which can be done by 
// comparing the current tick of the sim and the last update tick of the entity."

void asm__fixWeaponAim()
{
    asm(
                                               // default
        "movss xmm0, dword ptr ds:[eax+0x4];"  // targetPos.x
        "movss xmm1, dword ptr ss:[eax];"      // targetPos.z 
        "movss xmm2, dword ptr ss:[eax+0x8];"  // targetPos.y 
        
        "mov eax, dword ptr ds:[esi+0x4];"     // CAiTarget->entity
        "test eax, eax;"
        
        "je EXIT;"                             // no targetEnity when groundfiring
        
        "push ebx;"
        "cmp dword ptr ds:[esi+0x10], 0xF;"    // 0xF = auto fire, no attack order.
                                               // the route to entity->lastUpdateTick is different
        "je noAttackOrder;"                    // when there is an attack order
        
        "mov eax, dword ptr ds:[eax+0x5C];"
        "sub eax, 0x5C;"
        
        "noAttackOrder:;"
        "mov ebx, dword ptr ds:[eax+0x144];"
        "mov ebx, dword ptr ds:[ebx+0x900];"   // sim->currentTick
        "cmp ebx, dword ptr ds:[eax+0x170];"   // entity->lastUpdateTick
        "pop ebx;"
        
        "je EXIT;"                             // entityLastUpdateTick == currentSimTick means the target's pos is up-to-date
                                               // so no need to add target's velocity
        
        "addss xmm0, dword ptr ss:[esp+0x3C];" // targetVel.x
        "addss xmm1, dword ptr ss:[esp+0x38];" // targetVel.z
        "addss xmm2, dword ptr ss:[esp+0x40];" // targetVel.y
        
        "EXIT:;"
        "jmp 0x00631A92;"
    );
}