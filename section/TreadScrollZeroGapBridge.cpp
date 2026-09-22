// Stateful part of the tracked-unit tread-scroll fix. The phase rebase remains
// in TreadScrollPhaseSideFix.cpp; this file owns the complete mapped WorldMesh
// copy block and the bounded zero-gap fallback.
//
// Observed engine defect:
//   after confirmed movement, both source tread deltas can become exactly zero
//   for several simulation updates even though the unit continues moving. The
//   render path remains active and faithfully displays the zero source deltas.
//
// Policy:
//   - require the same WorldMesh and UserEntity pointers;
//   - require finite source deltas;
//   - require source phase continuity and two confirmed moving updates;
//   - bridge only simultaneous exact-zero deltas;
//   - reuse the last signed left/right deltas for at most six updates;
//   - on contention, collision uncertainty, invalid input or discontinuity,
//     immediately use the native mapped copy;
//   - preserve a bounded constant phase offset after recovery so native signed
//     deltas, direction and differential speed remain unchanged.
//
// Known review boundary:
//   a real instantaneous stop from meaningful speed is indistinguishable from
//   the observed source defect using these four source values alone. It can
//   therefore visually continue for at most six simulation updates. This
//   bounded visual trade-off was tested in game, judged minor, and remains
//   explicitly documented for maintainer review.

extern "C" const float TreadScrollPhaseInversePeriod
    __asm__("TreadScrollPhaseInversePeriod");
extern "C" const float TreadScrollPhasePeriod
    __asm__("TreadScrollPhasePeriod");

// 8192 direct-mapped 32-byte slots, fixed at build time (256 KiB).
// Layout per slot:
//   +0x00 WorldMesh key
//   +0x04 UserEntity key
//   +0x08 last mapped-left delta
//   +0x0C last mapped-right delta
//   +0x10 last source-left current bits  (UserEntity + 0xDC)
//   +0x14 last source-right current bits (UserEntity + 0xD8)
//   +0x18 flags and low-byte zero-gap count
//   +0x1C atomic slot lock
extern "C" {
volatile unsigned char TreadScrollZeroGapState[0x40000]
    __asm__("TreadScrollZeroGapState") __attribute__((aligned(32))) = {};
}

extern "C" __attribute__((naked)) void TreadScrollRebaseOne()
    __asm__("TreadScrollRebaseOne");
extern "C" __attribute__((naked)) void TreadScrollRebaseOne()
{
    asm volatile(
        // Input/output: XMM0. Scratch: EAX, EBX, XMM2.
        "MOVD EAX,XMM0;"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollRebaseOne_Done;"
        "MOVAPS XMM2,XMM0;"
        "MULSS XMM2,DWORD PTR [TreadScrollPhaseInversePeriod];"
        "CVTTSS2SI EBX,XMM2;"
        "TEST EBX,EBX;"
        "JE TreadScrollRebaseOne_Done;"
        "CVTSI2SS XMM2,EBX;"
        "MULSS XMM2,DWORD PTR [TreadScrollPhasePeriod];"
        "SUBSS XMM0,XMM2;"
        "TreadScrollRebaseOne_Done:;"
        "RET;"
    );
}

extern "C" __attribute__((naked)) void TreadScrollRebaseOutputPairs()
    __asm__("TreadScrollRebaseOutputPairs");
extern "C" __attribute__((naked)) void TreadScrollRebaseOutputPairs()
{
    asm volatile(
        // Inputs:
        //   EBP = WorldMesh
        //   EDX = locked state slot
        //   EDI = non-zero when phase continuity was confirmed
        //   XMM4 = mapped-left source delta
        // Preserves each previous/current difference exactly.
        "TEST EDI,EDI;"
        "JE TreadScrollRebaseOutputPairs_SaveDelta;"

        "MOV EAX,[EBP+0x9C];"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollRebaseOutputPairs_Right;"
        "MOVD XMM0,EAX;"
        "MULSS XMM0,DWORD PTR [TreadScrollPhaseInversePeriod];"
        "CVTTSS2SI EAX,XMM0;"
        "TEST EAX,EAX;"
        "JE TreadScrollRebaseOutputPairs_Right;"
        "CVTSI2SS XMM1,EAX;"
        "MULSS XMM1,DWORD PTR [TreadScrollPhasePeriod];"
        "MOVSS XMM0,DWORD PTR [EBP+0x94];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EBP+0x94],XMM0;"
        "MOVSS XMM0,DWORD PTR [EBP+0x9C];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM0;"

        "TreadScrollRebaseOutputPairs_Right:;"
        "MOV EAX,[EBP+0xA0];"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollRebaseOutputPairs_SaveDelta;"
        "MOVD XMM0,EAX;"
        "MULSS XMM0,DWORD PTR [TreadScrollPhaseInversePeriod];"
        "CVTTSS2SI EAX,XMM0;"
        "TEST EAX,EAX;"
        "JE TreadScrollRebaseOutputPairs_SaveDelta;"
        "CVTSI2SS XMM1,EAX;"
        "MULSS XMM1,DWORD PTR [TreadScrollPhasePeriod];"
        "MOVSS XMM0,DWORD PTR [EBP+0x98];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EBP+0x98],XMM0;"
        "MOVSS XMM0,DWORD PTR [EBP+0xA0];"
        "SUBSS XMM0,XMM1;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM0;"

        "TreadScrollRebaseOutputPairs_SaveDelta:;"
        "MOVSS DWORD PTR [EDX+0x08],XMM4;"
        "RET;"
    );
}

extern "C" __attribute__((naked)) void TreadScrollZeroGapBridge()
    __asm__("TreadScrollZeroGapBridge");
extern "C" __attribute__((naked)) void TreadScrollZeroGapBridge()
{
    asm volatile(
        // Original hook context:
        //   EDI = UserEntity
        //   EAX = WorldMesh ([EDI + 0x2C])
        // Original MOVSS block does not modify EFLAGS or general registers.
        "PUSHFD;"
        "PUSHAD;"
        "SUB ESP,0x50;"
        "MOVDQU [ESP],XMM4;"
        "MOVDQU [ESP+0x10],XMM5;"
        "MOVDQU [ESP+0x20],XMM6;"
        "MOVDQU [ESP+0x30],XMM7;"
        "STMXCSR [ESP+0x40];"

        "MOV EBP,EAX;"
        "MOV ESI,EDI;"

        // Mapped left = source channel B; mapped right = source channel A.
        "MOVSS XMM4,DWORD PTR [ESI+0xDC];"
        "SUBSS XMM4,DWORD PTR [ESI+0xD4];"
        "MOVSS XMM5,DWORD PTR [ESI+0xD8];"
        "SUBSS XMM5,DWORD PTR [ESI+0xD0];"

        // Hash both object pointers to an aligned 32-byte slot.
        "MOV EDX,EBP;"
        "XOR EDX,ESI;"
        "MOV EBX,EDX;"
        "SHR EDX,7;"
        "SHR EBX,19;"
        "XOR EDX,EBX;"
        "AND EDX,0x1FFF;"
        "SHL EDX,5;"
        "LEA EDX,[EDX+TreadScrollZeroGapState];"

        // Non-blocking slot ownership. Busy slots use the native path.
        "LOCK BTS DWORD PTR [EDX+0x1C],0;"
        "JC TreadScrollZeroGapBridge_NativeBusy;"

        "CMP [EDX+0x00],EBP;"
        "JNE TreadScrollZeroGapBridge_InitSlot;"
        "CMP [EDX+0x04],ESI;"
        "JNE TreadScrollZeroGapBridge_InitSlot;"

        // Branch to the zero-gap path only when both current deltas are exactly
        // +0.0 or -0.0.
        "MOVD EAX,XMM4;"
        "AND EAX,0x7FFFFFFF;"
        "JNE TreadScrollZeroGapBridge_NonZero;"
        "MOVD EAX,XMM5;"
        "AND EAX,0x7FFFFFFF;"
        "JNE TreadScrollZeroGapBridge_NonZero;"
        "JMP TreadScrollZeroGapBridge_Zero;"

        "TreadScrollZeroGapBridge_InitSlot:;"
        "MOV [EDX+0x00],EBP;"
        "MOV [EDX+0x04],ESI;"
        "MOVSS DWORD PTR [EDX+0x08],XMM4;"
        "MOVSS DWORD PTR [EDX+0x0C],XMM5;"
        "MOV EAX,[ESI+0xDC];"
        "MOV [EDX+0x10],EAX;"
        "MOV EAX,[ESI+0xD8];"
        "MOV [EDX+0x14],EAX;"
        "MOV DWORD PTR [EDX+0x18],0;"
        "JMP TreadScrollZeroGapBridge_NativeLocked;"

        "TreadScrollZeroGapBridge_NonZero:;"
        // Reject NaN and infinity in either source delta.
        "MOVD EAX,XMM4;"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollZeroGapBridge_ResetNative;"
        "MOVD EAX,XMM5;"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollZeroGapBridge_ResetNative;"

        // Exact source continuity: current previous values must equal the last
        // recorded current values for both channels.
        "XOR ECX,ECX;"
        "MOV EAX,[ESI+0xD4];"
        "CMP EAX,[EDX+0x10];"
        "JNE TreadScrollZeroGapBridge_ContinuityDone;"
        "MOV EAX,[ESI+0xD0];"
        "CMP EAX,[EDX+0x14];"
        "JNE TreadScrollZeroGapBridge_ContinuityDone;"
        "MOV ECX,1;"
        "TreadScrollZeroGapBridge_ContinuityDone:;"

        "MOV EAX,[EDX+0x18];"
        "MOV EDI,EAX;"
        "AND EDI,0xA0000000;"
        "TEST EDI,EDI;"
        "JE TreadScrollZeroGapBridge_CopyNativeMapped;"

        // Recover from a bridged phase by applying the same constant offset to
        // previous and current on each side. Native signed deltas are unchanged.
        "MOVSS XMM6,DWORD PTR [EBP+0x9C];"
        "SUBSS XMM6,DWORD PTR [ESI+0xD4];"
        "MOVSS XMM0,DWORD PTR [ESI+0xD4];"
        "ADDSS XMM0,XMM6;"
        "MOVSS XMM1,DWORD PTR [ESI+0xDC];"
        "ADDSS XMM1,XMM6;"
        "MOVSS DWORD PTR [EBP+0x94],XMM0;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM1;"

        "MOVSS XMM7,DWORD PTR [EBP+0xA0];"
        "SUBSS XMM7,DWORD PTR [ESI+0xD0];"
        "MOVSS XMM0,DWORD PTR [ESI+0xD0];"
        "ADDSS XMM0,XMM7;"
        "MOVSS XMM1,DWORD PTR [ESI+0xD8];"
        "ADDSS XMM1,XMM7;"
        "MOVSS DWORD PTR [EBP+0x98],XMM0;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM1;"
        "JMP TreadScrollZeroGapBridge_AfterMappedCopy;"

        "TreadScrollZeroGapBridge_CopyNativeMapped:;"
        "MOVSS XMM0,DWORD PTR [ESI+0xD8];"
        "MOVSS XMM1,DWORD PTR [ESI+0xDC];"
        "MOVSS XMM2,DWORD PTR [ESI+0xD0];"
        "MOVSS XMM3,DWORD PTR [ESI+0xD4];"
        "MOVSS DWORD PTR [EBP+0x94],XMM3;"
        "MOVSS DWORD PTR [EBP+0x98],XMM2;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM1;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM0;"

        "TreadScrollZeroGapBridge_AfterMappedCopy:;"
        "CALL TreadScrollRebaseOutputPairs;"
        "MOVSS DWORD PTR [EDX+0x0C],XMM5;"
        "MOV EAX,[ESI+0xDC];"
        "MOV [EDX+0x10],EAX;"
        "MOV EAX,[ESI+0xD8];"
        "MOV [EDX+0x14],EAX;"

        // Flags: 0x40000000 = continuous update; 0x20000000 = offset mode.
        "XOR EAX,EAX;"
        "TEST ECX,ECX;"
        "JE TreadScrollZeroGapBridge_SaveNonZeroFlags;"
        "OR EAX,0x40000000;"
        "TEST EDI,EDI;"
        "JE TreadScrollZeroGapBridge_SaveNonZeroFlags;"
        "OR EAX,0x20000000;"
        "TreadScrollZeroGapBridge_SaveNonZeroFlags:;"
        "MOV [EDX+0x18],EAX;"
        "JMP TreadScrollZeroGapBridge_UnlockExit;"

        "TreadScrollZeroGapBridge_Zero:;"
        "MOV EAX,[EDX+0x18];"
        "TEST EAX,0x40000000;"
        "JNE TreadScrollZeroGapBridge_CheckZeroContinuity;"
        "TEST EAX,0x80000000;"
        "JE TreadScrollZeroGapBridge_StopZero;"

        "TreadScrollZeroGapBridge_CheckZeroContinuity:;"
        "MOV EBX,[ESI+0xDC];"
        "CMP EBX,[EDX+0x10];"
        "JNE TreadScrollZeroGapBridge_StopZero;"
        "MOV EBX,[ESI+0xD8];"
        "CMP EBX,[EDX+0x14];"
        "JNE TreadScrollZeroGapBridge_StopZero;"

        // Last deltas must be finite and at least 0.01 in magnitude on one side.
        "MOV EAX,[EDX+0x08];"
        "MOV EBX,EAX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollZeroGapBridge_StopZero;"
        "AND EAX,0x7FFFFFFF;"
        "MOV ECX,[EDX+0x0C];"
        "MOV EBX,ECX;"
        "AND EBX,0x7F800000;"
        "CMP EBX,0x7F800000;"
        "JE TreadScrollZeroGapBridge_StopZero;"
        "AND ECX,0x7FFFFFFF;"
        "CMP EAX,ECX;"
        "CMOVB EAX,ECX;"
        "CMP EAX,0x3C23D70A;"
        "JB TreadScrollZeroGapBridge_StopZero;"

        // Maximum six bridged zero updates.
        "MOV EAX,[EDX+0x18];"
        "AND EAX,0xFF;"
        "CMP EAX,6;"
        "JAE TreadScrollZeroGapBridge_ZeroLimit;"
        "INC EAX;"
        "OR EAX,0xE0000000;"
        "MOV [EDX+0x18],EAX;"
        "MOVSS XMM4,DWORD PTR [EDX+0x08];"
        "MOVSS XMM5,DWORD PTR [EDX+0x0C];"

        "MOVSS XMM0,DWORD PTR [EBP+0x9C];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM6,XMM0;"
        "MOVSS XMM0,DWORD PTR [EBP+0xA0];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM7,XMM0;"
        "MOVSS DWORD PTR [EBP+0x94],XMM6;"
        "MOVSS DWORD PTR [EBP+0x98],XMM7;"
        "ADDSS XMM6,XMM4;"
        "ADDSS XMM7,XMM5;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM6;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM7;"
        "JMP TreadScrollZeroGapBridge_UnlockExit;"

        "TreadScrollZeroGapBridge_ZeroLimit:;"
        "MOV DWORD PTR [EDX+0x18],0xE0000006;"
        "MOVSS XMM0,DWORD PTR [EBP+0x9C];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM6,XMM0;"
        "MOVSS XMM0,DWORD PTR [EBP+0xA0];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM7,XMM0;"
        "MOVSS DWORD PTR [EBP+0x94],XMM6;"
        "MOVSS DWORD PTR [EBP+0x98],XMM7;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM6;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM7;"
        "JMP TreadScrollZeroGapBridge_UnlockExit;"

        "TreadScrollZeroGapBridge_StopZero:;"
        "MOV EAX,[EDX+0x18];"
        "TEST EAX,0x20000000;"
        "JE TreadScrollZeroGapBridge_StopZeroNative;"
        "MOV DWORD PTR [EDX+0x18],0x20000000;"
        "MOV DWORD PTR [EDX+0x08],0;"
        "MOV DWORD PTR [EDX+0x0C],0;"
        "MOVSS XMM0,DWORD PTR [EBP+0x9C];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM6,XMM0;"
        "MOVSS XMM0,DWORD PTR [EBP+0xA0];"
        "CALL TreadScrollRebaseOne;"
        "MOVAPS XMM7,XMM0;"
        "MOVSS DWORD PTR [EBP+0x94],XMM6;"
        "MOVSS DWORD PTR [EBP+0x98],XMM7;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM6;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM7;"
        "JMP TreadScrollZeroGapBridge_UnlockExit;"

        "TreadScrollZeroGapBridge_StopZeroNative:;"
        "MOV DWORD PTR [EDX+0x18],0;"
        "MOV DWORD PTR [EDX+0x08],0;"
        "MOV DWORD PTR [EDX+0x0C],0;"
        "JMP TreadScrollZeroGapBridge_NativeLocked;"

        "TreadScrollZeroGapBridge_ResetNative:;"
        "MOV DWORD PTR [EDX+0x18],0;"
        "MOV DWORD PTR [EDX+0x08],0;"
        "MOV DWORD PTR [EDX+0x0C],0;"

        "TreadScrollZeroGapBridge_NativeLocked:;"
        "MOVSS XMM0,DWORD PTR [ESI+0xD8];"
        "MOVSS XMM1,DWORD PTR [ESI+0xDC];"
        "MOVSS XMM2,DWORD PTR [ESI+0xD0];"
        "MOVSS XMM3,DWORD PTR [ESI+0xD4];"
        "MOVSS DWORD PTR [EBP+0x94],XMM3;"
        "MOVSS DWORD PTR [EBP+0x98],XMM2;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM1;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM0;"
        "JMP TreadScrollZeroGapBridge_UnlockExit;"

        "TreadScrollZeroGapBridge_NativeBusy:;"
        "MOVSS XMM0,DWORD PTR [ESI+0xD8];"
        "MOVSS XMM1,DWORD PTR [ESI+0xDC];"
        "MOVSS XMM2,DWORD PTR [ESI+0xD0];"
        "MOVSS XMM3,DWORD PTR [ESI+0xD4];"
        "MOVSS DWORD PTR [EBP+0x94],XMM3;"
        "MOVSS DWORD PTR [EBP+0x98],XMM2;"
        "MOVSS DWORD PTR [EBP+0x9C],XMM1;"
        "MOVSS DWORD PTR [EBP+0xA0],XMM0;"
        "JMP TreadScrollZeroGapBridge_Exit;"

        "TreadScrollZeroGapBridge_UnlockExit:;"
        "MOV DWORD PTR [EDX+0x1C],0;"

        "TreadScrollZeroGapBridge_Exit:;"
        // Restore the XMM0-XMM3 values produced by the original MOVSS loads.
        "MOVSS XMM0,DWORD PTR [ESI+0xD8];"
        "MOVSS XMM1,DWORD PTR [ESI+0xDC];"
        "MOVSS XMM2,DWORD PTR [ESI+0xD0];"
        "MOVSS XMM3,DWORD PTR [ESI+0xD4];"
        "LDMXCSR [ESP+0x40];"
        "MOVDQU XMM4,[ESP];"
        "MOVDQU XMM5,[ESP+0x10];"
        "MOVDQU XMM6,[ESP+0x20];"
        "MOVDQU XMM7,[ESP+0x30];"
        "ADD ESP,0x50;"
        "POPAD;"
        "POPFD;"
        "JMP 0x008B92C4;"
    );
}
