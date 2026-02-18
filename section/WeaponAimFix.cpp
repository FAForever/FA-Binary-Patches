// Fixes aim manipulators aiming 1 tick ahead of entities that have already updated their position.
void asm__fixWeaponAim()
{
    // inputs:
    // eax = WeakObject<Entity> *p_EntityWeakObj (Entity + 0x4 *)
    // esp+0x44 = Vector3f vec3f
    asm(
        "lea ecx, ds:[eax-4];"      // ecx = p_EntityWeakObj->GetObject();
        "mov eax, ds:[ecx+0x148];"  // eax = Entity.sim;
        "mov eax, ds:[eax+0x900];"  // eax = Sim.cur_tick;
        "mov edx, ds:[ecx+0x174];"  // edx = Entity.lastUpdateTick;
        "cmp eax, edx;"             // if cur_tick == lastUpdateTick
        "je 0x00631A2F;"            // then output 0-vector and end
        "mov eax, ds:[ecx];"        // eax = Entity.__vftable;
        "mov eax, ds:[eax+0x3C];"   // eax = Entity_vtbl.GetVelocity;
        "lea edx, ds:[esp+0x44];"   // edx = &vec3f
        "push edx;"                 // ^0.4 = &vec3f
        "call eax;"                 // eax = GetVelocity(Entity@<ecx>, Vector3f *@<^0.4>)
        "jmp 0x00631A45;"           // end
    );
    // outputs:
    // eax = Vector3f *targetVelocityToAdd
}