
asm(R"(

    one = 0xDFEC20
    two = 0xDFEB0C

#Moho::CAiFormationInstance::CAiFormationInstance at 0x05694B0
.section h1; .set h1,0x05696B3 #Moho::CAiFormationInstance::CAiFormationInstance+0x203
    # Slide up everything up and over the setting of the useless function position
    # variable to `6` to make room for setting the max size to `1`
    # (which takes 4 more bytes than to set it to `esi`, which holds `0`)
    mov     ecx, [eax]
    movss   xmm0, dword ptr [one]
    mov     dword ptr [edi+0x310], ecx
    mov     edx, [eax+4]
    movss   dword ptr [edi+0x318], xmm0
    mov     dword ptr [edi+0x320], 1  # Set maximum footprint size to 1
    nop


#Moho::CAiFormationInstance::UpdateFormation at 0x0568CA0
    orientXDir      = -0x4C
    orientYDir      = -0x44
.section h2; .set h2,0x0568DC6 #Moho::CAiFormationInstance::UpdateFormation+0x126
    # remove the blueprint checks for size, which begins interlacing in the
    # orientation averaging code here
    mulss   xmm0, xmm1
    mulss   xmm3, xmm2
    movaps  xmm4, xmm2
    mulss   xmm4, xmm2
    movaps  xmm2, xmm1
    mulss   xmm2, xmm1
    movss   xmm1, dword ptr [one]
    addss   xmm0, xmm3
    movss   xmm3, dword ptr [two]
    addss   xmm4, xmm2
    mulss   xmm4, xmm3
    mulss   xmm0, xmm3
    addss   xmm0, [esp+0x68+orientXDir]
    subss   xmm1, xmm4
    addss   xmm1, [esp+0x68+orientYDir]
    movss   [esp+0x68+orientXDir], xmm0
    movss   [esp+0x68+orientYDir], xmm1
    jmp     0x0568E8F

)");
