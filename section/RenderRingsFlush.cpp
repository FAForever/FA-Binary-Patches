// Intermediate RangeMask flush to prevent 7-bit stencil counter overflow
// in the first Cast loop of func_RenderRings (0x007EF5A0).
//
// === Why this exists ===
//
// func_RenderRings draws range rings via two loops, both calling
// func_Draw_Rings with the same "Cast" effect technique. That technique
// increments the GPU stencil per overdraw layer, then later passes use
// the stencil count for early-out culling and edge masking. The stencil
// is 7-bit (max 127). With 128+ overlapping rings the counter wraps and
// the visual collapses to individual circle outlines instead of a merged
// shape.
//
// Loop 1 (0x007EF774-0x007EF7FC): draws the FILL band of each ring.
//   sub_7EDC80 emits 1 quad per ring covering the area between
//   inner-radius+thickness and outer-radius-thickness. High per-pixel
//   overdraw at dense build sites -- this is the loop that overflows.
//
// Loop 2 (0x007EF861-0x007EF8D5): draws the two thin EDGE strips per ring.
//   sub_7EDC80 emits 2 quads per ring at the inner and outer borders.
//   Near-zero per-pixel overdraw because edges of differently-sized rings
//   almost never coincide -- does not need patching in practice.
//
// Fix: drop Loop 1 batch size 1000 -> 30 and inject this flush between
// batches. The flush replays the engine's own end-of-loop sequence
// (InitTransformedVerts -> RangeMask string -> Render) which clears the
// stencil and reapplies the mask before the next batch starts fresh.
//
// === Calling conventions ===
//
// Despite the IDA mangled-name display showing standard __thiscall, this
// build uses a custom register-based thiscall for the CRenFrame methods
// (verified by inspecting the function bodies and the engine's own call
// sites at 0x007EF820-0x007EF849). A direct C++ call is impossible
// because no C++ calling convention puts `this` in EBX or EDI, so the
// flush is implemented in raw asm.
//
//   0x007F5DA0  Moho::CRenFrame::InitTransformedVerts
//                 this @ EBX, (float w, float h), retn 8
//                 first usage at 0x007F5DD1: cmp [ebx+0x1C], 0
//
//   0x004059E0  std::string::string (the engine builds an std::string
//               in-place at the technique slot to select "RangeMask")
//                 this @ ECX, (char const* s, size_t n), retn 8
//                 standard MSVC __thiscall
//
//   0x007F6030  Moho::CRenFrame::Render
//                 this @ EDI, (int w, int h), retn 8
//                 first usage at 0x007F6055: cmp [edi+0x18], 0x10
//
// === Stack layout ===
//
// At entry esp = caller_esp - 4 (return address pushed by the call hook).
// After pushad esp = caller_esp - 36. The hook is injected at 0x007EF7EA
// inside func_RenderRings where its esp delta from frame base puts:
//
//   [caller_esp + 0x78] = idxa (gpg::gal::Head*) -- needed for width/height
//   [caller_esp + 0x74] = arg_0 (CRenFrame container, +0x4C = technique ptr)
//
// Adding 36 for the pushad/return-address gives the offsets used below.
//
// The instruction replaced at 0x007EF7EA is the 7-byte
// `mov eax, [esp+0x84]` (= load loop counter `i`). It is reproduced after
// popad as `mov eax, [esp+0x88]` because esp is still 4 bytes lower from
// the call's return address being pushed.

void IntermediateRangeMask()
{
    asm(
        "pushad;"

        // esi = idxa (Head*), edi = technique ptr
        "mov esi, dword ptr [esp+0x9C];"  // [caller+0x78]
        "mov edi, dword ptr [esp+0x98];"  // [caller+0x74]
        "add edi, 0x4C;"

        // --- InitTransformedVerts(this=ebx, width, height) ---
        "cvtsi2ss xmm0, dword ptr [esi+0x14];"
        "sub esp, 8;"
        "movss dword ptr [esp+4], xmm0;"
        "cvtsi2ss xmm0, dword ptr [esi+0x10];"
        "mov ebx, edi;"                    // this @ ebx
        "movss dword ptr [esp], xmm0;"
        "call 0x007F5DA0;"

        // --- std::string::string(this=ecx, "RangeMask", 9) ---
        "push 9;"
        "push 0x00E3F8E8;"                 // address of "RangeMask" string
        "mov ecx, edi;"                    // this @ ecx
        "call 0x004059E0;"

        // --- Render(this=edi, width, height) ---
        "mov ecx, dword ptr [esi+0x14];"
        "mov edx, dword ptr [esi+0x10];"
        "push ecx;"
        "push edx;"
        "call 0x007F6030;"                 // this @ edi (preserved through above)

        "popad;"
        // Reproduce the 7-byte instruction we overwrote at 0x007EF7EA.
        // esp is still -4 from the call's return address, so [esp+0x88]
        // here equals [original_esp+0x84] in the host function.
        "mov eax, dword ptr [esp+0x88];"
        "ret;"
    );
}
