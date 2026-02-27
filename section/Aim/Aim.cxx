#include "global.h"
#include "Maths.h"

namespace Moho
{
    // requires dtor!!!
    struct CAiTarget
    {
        int targetType;
        void *entity;
        void *next_weak_ptr;
        Vector3f position;
        int targetPoint;
        bool targetIsMobile;

        bool HasTargetPoint() const { return targetPoint != -1; }
    };
}

SHARED __thiscall bool CheckNeedAddEntityVelocity(Moho::CAiTarget *cai_target)
{
    void *entity = cai_target->entity;
    if (entity == 0 || entity == (void *)4)
        return false;

    entity = Offset<void *>(entity, -4);
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
    if (CheckNeedAddEntityVelocity(cai_target) && cai_target->HasTargetPoint())
        aim_pos = target_pos + target_velocity;
    else
        aim_pos = target_pos;

    return std::sqrt(
        (aim_pos.x - shooter_pos.x) * (aim_pos.x - shooter_pos.x) +
        // (aim_pos.y - shooter_pos.y) * (aim_pos.y - shooter_pos.y) +
        (aim_pos.z - shooter_pos.z) * (aim_pos.z - shooter_pos.z));
}