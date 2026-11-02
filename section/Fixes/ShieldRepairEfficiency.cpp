// Shield repair amount was regenRate * buildRate / regenAssistMult regardless of
// how much of its requested resources the builder got. Scale it by the builder's
// ResourceConsumed like build progress is.
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
