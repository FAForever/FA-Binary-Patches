// Projectile::CheckCollision finds no terrain outside the map, so projectiles
// that leave it fall forever. Treat the terrain as extending outwards from the
// map edge: when the swept segment ends outside the map below the terrain height
// of the nearest map point, record a terrain hit there.
// CHeightField::GetElevation clamps its samples to the grid, so outside the map it
// already returns the height of the nearest edge.
//
// Reached from hooks/FixCollisions.cpp when the terrain intersection misses, so
// only projectiles that collide with the surface get here. The hit is recorded
// like a terrain intersection hit: it only replaces a closer queued impact, and
// it cancels the water layer change if it comes before the water surface.
//
// In: edi = this, esp = CheckCollision's frame, xmm3 = water crossing distance
// (kept for SPLICE).
extern "C" void ProjectileMapEdgeCollision()
{
    asm(
    "_ProjectileMapEdgeCollision:;"
    "mov     eax, [esp+0x230-0x208];"      // mapData (STIMap)
    "mov     ecx, [eax];"                  // STIMap->heightField
    "movss   xmm0, [edi+0x160];"           // nextPos.x (pendingTransform.pos)
    "movss   xmm1, [edi+0x168];"           // nextPos.z
    "xorps   xmm2, xmm2;"
    "comiss  xmm0, xmm2;"
    "jb      .L_OUTSIDE;"                  // x < 0
    "comiss  xmm1, xmm2;"
    "jb      .L_OUTSIDE;"                  // z < 0
    "mov     eax, [ecx+0x4];"              // heightField->width (samples, map size + 1)
    "dec     eax;"
    "cvtsi2ss xmm2, eax;"
    "comiss  xmm0, xmm2;"
    "ja      .L_OUTSIDE;"                  // x > map size x
    "mov     eax, [ecx+0x8];"              // heightField->height
    "dec     eax;"
    "cvtsi2ss xmm2, eax;"
    "comiss  xmm1, xmm2;"
    "ja      .L_OUTSIDE;"                  // z > map size z
    ".L_NO_HIT:;"
    "jmp     0x0069DB59;"                  // CHECK_LAYER_CHANGE

    ".L_OUTSIDE:;"
    "sub     esp, 0x8;"
    "movss   [esp], xmm0;"
    "movss   [esp+0x4], xmm1;"
    "call    0x0044FB90;"                  // CHeightField::GetElevation(x, z), pops its args, keeps xmm3
    "push    eax;"
    "fstp    dword ptr [esp];"
    "movss   xmm0, [esp];"                 // xmm0 = edge elevation
    "add     esp, 0x4;"
    "movss   xmm1, [edi+0x164];"           // nextPos.y
    "comiss  xmm0, xmm1;"
    "jbe     .L_NO_HIT;"                   // ends above the edge terrain

    // fraction of the segment where it crosses the edge height; 0 when it
    // started below it
    "movss   xmm2, [edi+0xB0];"            // curPos.y (curTransform.pos)
    "xorps   xmm4, xmm4;"
    "comiss  xmm2, xmm0;"
    "jbe     .L_HAVE_FRACTION;"
    "movaps  xmm4, xmm2;"
    "subss   xmm4, xmm0;"                  // curPos.y - elevation
    "subss   xmm2, xmm1;"                  // curPos.y - nextPos.y, > 0 here
    "divss   xmm4, xmm2;"
    ".L_HAVE_FRACTION:;"

    // only replace a queued impact that comes later along the segment
    "movss   xmm1, [edi+0x2A4];"           // impactInterp
    "xorps   xmm2, xmm2;"
    "comiss  xmm2, xmm1;"
    "ja      .L_RECORD;"                   // nothing queued
    "comiss  xmm1, xmm4;"
    "jbe     .L_NO_HIT;"
    ".L_RECORD:;"
    "movss   [edi+0x2A4], xmm4;"           // impactInterp = fraction
    "movss   xmm1, [edi+0x160];"           // impactPosition = lerp(curPos, nextPos), y = elevation
    "subss   xmm1, [edi+0xAC];"
    "mulss   xmm1, xmm4;"
    "addss   xmm1, [edi+0xAC];"
    "movss   [edi+0x31C], xmm1;"
    "movss   [edi+0x320], xmm0;"
    "movss   xmm1, [edi+0x168];"
    "subss   xmm1, [edi+0xB4];"
    "mulss   xmm1, xmm4;"
    "addss   xmm1, [edi+0xB4];"
    "movss   [edi+0x324], xmm1;"
    "movaps  xmm2, xmm4;"
    "mulss   xmm2, [esp+0x230-0x220];"     // hit distance = fraction * distMoved, for the water comparison
    "jmp     0x0069DB46;"                  // SPLICE: impactType = IMPACT_Terrain
    );
}
