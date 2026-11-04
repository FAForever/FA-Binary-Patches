// Projectiles that leave the map never hit the ground: the terrain intersection
// in Projectile::CheckCollision finds nothing outside the height field, so they
// fall forever. Treat the terrain as extending outwards from the map edge:
// outside the map, a projectile below the terrain height of the nearest map point
// impacts the terrain there. CHeightField::GetElevation clamps its samples to the
// grid, so outside the map it already returns the height of the nearest edge.
//
// Only projectiles that collide with the surface are checked, so effect
// projectiles that are meant to go through the ground are left alone. Water
// outside the map needs nothing here: CheckCollision's water plane test is not
// limited to the map.
void asm__ProjMotionTick()
{
    asm(
    "push    ebp;"
    "push    esi;"
    "push    edi;"
    "mov     ebp, ecx;"
    // an impact queued last tick goes first, so its type isn't overwritten
    "movss   xmm0, dword ptr [ebp+0x2A4];" // this->impactInterpolation
    "comiss  xmm0, ds:0xE4F6E0;"           // 0.0f
    "jae     0x0069BDF0;"                  // Impact()
    "cmp     byte ptr [ebp+0x2A8], 0;"     // this->collideSurface
    "je      0x0069BE04;"                  // continue moving
    "mov     eax, dword ptr [ebp+0x148];"  // this->sim
    "mov     eax, dword ptr [eax+0x8CC];"  // sim->STIMap
    "mov     ecx, dword ptr [eax];"        // STIMap->heightField
    "movss   xmm0, dword ptr [ebp+0xAC];"  // this->curTransform.pos.x
    "movss   xmm1, dword ptr [ebp+0xB4];"  // this->curTransform.pos.z
    "xorps   xmm2, xmm2;"
    "comiss  xmm0, xmm2;"
    "jb      1f;"                          // x < 0
    "comiss  xmm1, xmm2;"
    "jb      1f;"                          // z < 0
    "mov     eax, dword ptr [ecx+0x4];"    // heightField->width (samples, map size + 1)
    "dec     eax;"
    "cvtsi2ss xmm2, eax;"
    "comiss  xmm0, xmm2;"
    "ja      1f;"                          // x > map size x
    "mov     eax, dword ptr [ecx+0x8];"    // heightField->height
    "dec     eax;"
    "cvtsi2ss xmm2, eax;"
    "comiss  xmm1, xmm2;"
    "jbe     0x0069BE04;"                  // inside the map: continue moving
    "1:"
    "sub     esp, 0x8;"
    "movss   dword ptr [esp], xmm0;"
    "movss   dword ptr [esp+0x4], xmm1;"
    "call    0x0044FB90;"                  // CHeightField::GetElevation(x, z), pops its args
    "push    eax;"
    "fstp    dword ptr [esp];"
    "movss   xmm0, dword ptr [esp];"
    "add     esp, 0x4;"
    "comiss  xmm0, dword ptr [ebp+0xB0];"  // edge elevation > pos.y
    "jbe     0x0069BE04;"                  // above the edge: continue moving
    // impact the terrain at the edge height straight above the projectile
    "mov     eax, dword ptr [ebp+0xAC];"
    "mov     dword ptr [ebp+0x31C], eax;"  // this->impactPosition.x
    "movss   dword ptr [ebp+0x320], xmm0;" // this->impactPosition.y
    "mov     eax, dword ptr [ebp+0xB4];"
    "mov     dword ptr [ebp+0x324], eax;"  // this->impactPosition.z
    "mov     dword ptr [ebp+0x364], 1;"    // this->impactType = IMPACT_Terrain
    "jmp     0x0069BDF0;"                  // Impact()
        :
        :
        :);
}
