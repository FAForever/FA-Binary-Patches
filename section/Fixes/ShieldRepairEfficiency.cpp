// Shield repair amount was regenRate * buildRate / regenAssistMult regardless of
// how much of its requested resources the builder got. Scale it by the builder's
// ResourceConsumed like build progress is. Not by build progress itself: that is
// also divided by the unit's BuildTime, and halved again for a damaged unit, on
// top of the regenAssistMult doubling that already splits buildpower 1/2.
void ShieldRepairEfficiency()
{
    asm(
    "movss   xmm1, [esp+0x20];"     // regenRate * 0.1
    "mulss   xmm1, [esp+0x18];"     // * builder buildRate
    "mov     eax, [ebp];"           // this->ownerUnit
    "mulss   xmm1, [eax+0x53C];"    // * ownerUnit->ResourceConsumed
    "jmp     0x5F60B9;"
        :
        :
        :);
}
