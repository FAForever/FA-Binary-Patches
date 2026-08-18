void asm__ProjMotionTick()
{
    asm(
    "push    ebp;"
    "push    esi;"
    "push    edi;"
    "mov     ebp, ecx;"
    "movss   xmm0, dword ptr [ebp+0x78 + 0x24 + 0x10 + 0x4];" // this->sstiData.curTransform.pos.y
    "comiss  xmm0, ds:0xE4F6E0;" // if y < 0
    // then
    "jb  0x0069BDF0;" // on impact
    // else
    "jmp 0x0069BDDF;"
        :
        :
        :);
}