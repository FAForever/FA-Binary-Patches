#include "global.h"
#include "Wm3.h"

namespace Moho
{
    // requires dtor!!!
    struct CAiTarget
    {
        int targetType;
        void *entity;
        void *next_weak_ptr;
        Vector3f position;
        int targetBoneIdx;
        bool targetIsMobile;
    };
}


// When targeted manually cai_target uses blip and when not - unit's entity
SHARED __thiscall bool CheckNeedAddEntityVelocity(Moho::CAiTarget *cai_target)
{
    void *entity = cai_target->entity;
    if (entity == 0 || entity == (void *)4)
        return false;

    entity = Offset(entity, -4);

    void *vtable = GetField(entity, 0x0);
    void *__thiscall (*IsBlip)(void *) = GetField<void *__thiscall (*)(void *entity)>(vtable, 28);
    void *blip = IsBlip(entity);

    if (blip)
    {
        void *unit_creator_ptr = GetField<void *>(blip, 0x270);
        if (unit_creator_ptr == nullptr || unit_creator_ptr == (void *)4)
            return false;
        void *unit_creator = Offset(unit_creator_ptr, -4);
        entity = Offset(unit_creator, 8);
    }

    int update_tick = GetField<int>(entity, 0x174);
    void *sim = GetField<void *>(entity, 0x148);
    int sim_tick = GetField<int>(sim, 0x900);
    if (sim_tick == update_tick)
        return false;

    return true;
}

#include <cmath>

SHARED __thiscall float AimAddVelocity(
    Moho::CAiTarget *cai_target,
    Vector3f &aim_pos,
    const Vector3f &shooter_pos,
    const Vector3f &target_velocity,
    const Vector3f &target_pos)
{
    if (CheckNeedAddEntityVelocity(cai_target))
        aim_pos = target_pos + target_velocity;
    else
        aim_pos = target_pos;

    // aim_pos = target_pos + target_velocity;
    return std::sqrt(
        (aim_pos.x - shooter_pos.x) * (aim_pos.x - shooter_pos.x) +
        // (aim_pos.y - shooter_pos.y) * (aim_pos.y - shooter_pos.y) +
        (aim_pos.z - shooter_pos.z) * (aim_pos.z - shooter_pos.z));
}