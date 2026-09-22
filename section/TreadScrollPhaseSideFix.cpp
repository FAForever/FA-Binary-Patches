// Keep the two UV scroll phases in a small exact range while preserving the
// complete signed delta of each channel:
//
//   q = trunc(current / 16)
//   previous -= q * 16
//   current  -= q * 16
//
// The UV layout repeats every 16 phase units. Rebasing previous and current by
// the same integer multiple therefore leaves direction, speed and interpolation
// unchanged, while avoiding float precision loss during long sessions.

// GCC emits these file-scope constants in reverse declaration order with the
// repository build flags. Keep this declaration order so the linked layout is
// inverse period first, period second, matching the validated v3.1 build.
extern "C" const float TreadScrollPhasePeriod
    __asm__("TreadScrollPhasePeriod") = 16.0f;
extern "C" const float TreadScrollPhaseInversePeriod
    __asm__("TreadScrollPhaseInversePeriod") = 0.0625f;

extern "C" __attribute__((naked)) void TreadScrollPhaseRebase()
{
    asm volatile(
        // Preserve the pre-hook EFLAGS, MXCSR and XMM0/XMM1 while the
        // injected arithmetic runs. The replayed CALL's caller-clobbered
        // EAX/ECX/EDX/EFLAGS/XMM results are dead at 0x008B903D: each is
        // overwritten before any caller-side read (see the liveness audit).
        "PUSHFD;"
        "SUB ESP,0x24;"
        "MOVUPS [ESP],XMM0;"
        "MOVUPS [ESP+0x10],XMM1;"
        "STMXCSR [ESP+0x20];"

        // Replay the original instructions.
        "LEA EBX,[EDI+0x50];"
        "MOV EAX,ESI;"
        "CALL 0x0067A3E0;"

        // Channel A: previous +0xD0, current +0xD8.
        "MOV ECX,[EDI+0xD8];"
        "MOV EDX,ECX;"
        "AND EDX,0x7F800000;"
        "CMP EDX,0x7F800000;"
        "JE TreadScrollPhaseRebase_ChannelB;"
        "MOVD XMM0,ECX;"
        "MULSS XMM0,DWORD PTR [TreadScrollPhaseInversePeriod];"
        "CVTTSS2SI ECX,XMM0;"
        "TEST ECX,ECX;"
        "JE TreadScrollPhaseRebase_ChannelB;"
        "CVTSI2SS XMM1,ECX;"
        "MULSS XMM1,DWORD PTR [TreadScrollPhasePeriod];"
        "MOVSS XMM0,DWORD PTR [EDI+0xD8];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EDI+0xD8],XMM0;"
        "MOVSS XMM0,DWORD PTR [EDI+0xD0];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EDI+0xD0],XMM0;"

        // Channel B: previous +0xD4, current +0xDC.
        "TreadScrollPhaseRebase_ChannelB:;"
        "MOV ECX,[EDI+0xDC];"
        "MOV EDX,ECX;"
        "AND EDX,0x7F800000;"
        "CMP EDX,0x7F800000;"
        "JE TreadScrollPhaseRebase_Done;"
        "MOVD XMM0,ECX;"
        "MULSS XMM0,DWORD PTR [TreadScrollPhaseInversePeriod];"
        "CVTTSS2SI ECX,XMM0;"
        "TEST ECX,ECX;"
        "JE TreadScrollPhaseRebase_Done;"
        "CVTSI2SS XMM1,ECX;"
        "MULSS XMM1,DWORD PTR [TreadScrollPhasePeriod];"
        "MOVSS XMM0,DWORD PTR [EDI+0xDC];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EDI+0xDC],XMM0;"
        "MOVSS XMM0,DWORD PTR [EDI+0xD4];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EDI+0xD4],XMM0;"

        "TreadScrollPhaseRebase_Done:;"
        "LDMXCSR [ESP+0x20];"
        "MOVUPS XMM0,[ESP];"
        "MOVUPS XMM1,[ESP+0x10];"
        "ADD ESP,0x24;"
        "POPFD;"

        // The original callee returns EAX = EBX. Other caller-clobbered
        // return values are dead at the resume point.
        "MOV EAX,EBX;"
        "JMP 0x008B903D;"
    );
}
