#include "magic_classes.h"

// Hull culling for range rings.
//
// func_RenderRings (0x007EF5A0) gets a vector of N ring positions and
// renders every single one. With dense crowds (600+ units) the per-ring
// setup cost dominates frame time.
//
// This patch hooks at 0x007EF5E2 and runs a greedy compaction pass that
// keeps only units whose outer-circle samples are NOT fully covered by
// already-kept neighbours. Interior units get dropped; boundary and
// isolated units survive. The engine's stencil-based outline rendering
// then produces the same merged hull from fewer rings.
//
// Disabled by default. Enable: `ui_RangeRingClusterHull 1`.

float g_RingClusterHull = 0.0f;

ConDescReg ring_cluster_hull_reg{
    "ui_RangeRingClusterHull",
    "If non-zero, cull range rings whose outer circle is fully covered by "
    "already-kept neighbours (greedy 16-sample geometric test). Only boundary "
    "and isolated units render. Large FPS gain in dense crowds.",
    &g_RingClusterHull};

// Each ring position entry in the vector is 16 bytes laid out as 4 floats.
// Layout was verified by reading Moho::WeaponExtractor::Range (0x7EC650),
// which writes the buffer that gets pushed onto the position vector via
// sub_7F0310 in func_ExtractRanges:
//
//   [0] = world X
//   [1] = world Z   (NOT Y -- SCFA uses Y as elevation, the ground plane
//                    is XZ)
//   [2] = inner radius  (= min weapon range across all weapons of the
//                         unit's category, can be 0 for solid disks)
//   [3] = outer radius  (= max weapon range across all weapons of the
//                         unit's category, always > 0)
//
// Multiple units of DIFFERENT sizes can co-exist in a single
// func_RenderRings call: func_ExtractRanges iterates every army unit
// through one shared RangeExtractor, so e.g. an ACU's main gun and a T1
// bot's main gun both end up in the "Direct Fire" call with their own
// per-entry [innerR, outerR].
//
// === Greedy hull culling ===
//
// The naive "for each unit, check if covered by ALL OTHER units" test has
// a fatal flaw: it can cull units whose covering neighbours will THEMSELVES
// be culled, leaving the surviving kept-set with holes that don't actually
// cover the original union. The visual symptom is fragmented partial-ring
// outlines inside the cluster (kept boundary units' inward edges showing
// through gaps in the stencil mask where culled units used to fill).
//
// The fix is to make the test order-aware: a candidate P is checked only
// against the ALREADY-KEPT set (not against units that may yet be culled).
// We compact the input vector in place during the same pass:
//
//   * data[0 .. writeIdx)  is the keep set so far
//   * data[i]              is the candidate being inspected
//   * If P is fully covered by data[0..writeIdx), drop it
//   * Otherwise copy P to data[writeIdx] and bump writeIdx
//
// This guarantees: every kept unit is NOT covered by other kept units, AND
// every culled unit IS fully covered by some subset of kept units (and
// therefore by the entire kept set). The keep-set's stencil mask is then
// equivalent to the full original mask.
//
// Coverage test: 24 samples (every 15 degrees) on P's outer circle. Each
// sample must lie in some kept neighbour's [innerR, outerR] band. Outer
// loop early-exits on first uncovered sample.
extern "C" int ClusterRingPositions(float *data, int count)
{
    if (g_RingClusterHull == 0.0f || count <= 4) return count;

    // 24 unit vectors at 15-degree intervals around the outer circle.
    // The engine draws rings as 45 segments (8 deg each); 24 samples at
    // 15 deg catches gaps down to ~2 segments, reducing outline choppiness.
    static const float COSDIR[24] = {
         1.00000000f,  0.96592583f,  0.86602540f,  0.70710677f,
         0.50000000f,  0.25881905f,  0.00000000f, -0.25881905f,
        -0.50000000f, -0.70710677f, -0.86602540f, -0.96592583f,
        -1.00000000f, -0.96592583f, -0.86602540f, -0.70710677f,
        -0.50000000f, -0.25881905f,  0.00000000f,  0.25881905f,
         0.50000000f,  0.70710677f,  0.86602540f,  0.96592583f
    };
    static const float SINDIR[24] = {
         0.00000000f,  0.25881905f,  0.50000000f,  0.70710677f,
         0.86602540f,  0.96592583f,  1.00000000f,  0.96592583f,
         0.86602540f,  0.70710677f,  0.50000000f,  0.25881905f,
         0.00000000f, -0.25881905f, -0.50000000f, -0.70710677f,
        -0.86602540f, -0.96592583f, -1.00000000f, -0.96592583f,
        -0.86602540f, -0.70710677f, -0.50000000f, -0.25881905f
    };

    int writeIdx = 0;

    for (int i = 0; i < count; ++i) {
        // Snapshot the candidate locally so it survives any in-place
        // overwrite at data[writeIdx] further down.
        float px     = data[i * 4 + 0];
        float pz     = data[i * 4 + 1];
        float pInner = data[i * 4 + 2];
        float pOuter = data[i * 4 + 3];

        // First unit always kept (nothing to be covered by).
        bool fully_covered = (writeIdx > 0);

        for (int k = 0; k < 24 && fully_covered; ++k) {
            float tx = px + pOuter * COSDIR[k];
            float tz = pz + pOuter * SINDIR[k];

            bool sample_covered = false;
            for (int j = 0; j < writeIdx; ++j) {  // ONLY already-kept set
                float *Q = data + j * 4;
                float dx = Q[0] - tx;
                float dz = Q[1] - tz;
                float distSq = dx * dx + dz * dz;

                float qOuter = Q[3];
                if (distSq > qOuter * qOuter) continue;

                float qInner = Q[2];
                if (qInner > 0.0f && distSq < qInner * qInner) continue;

                sample_covered = true;
                break;
            }

            if (!sample_covered) {
                fully_covered = false;
            }
        }

        if (!fully_covered) {
            float *dest = data + writeIdx * 4;
            dest[0] = px;
            dest[1] = pz;
            dest[2] = pInner;
            dest[3] = pOuter;
            writeIdx++;
        }
    }

    return writeIdx;
}

// Trampoline installed at 0x007EF5E2 (replaces the 5-byte
// `mov eax, ds:0x10a6438` -- sWldMap -- which we re-execute on the way out).
//
// Live registers at the patch site, deduced from the surrounding asm:
//   ecx  = vector pointer (last arg `i` to func_RenderRings)
//          [ecx+4] = data start, [ecx+8] = data end
//   ebp  = ring count (= (end - start) >> 4), already shifted, non-zero
//   esi  = edx0 arg (Camera-related), must preserve
//   ebx  = saved original ecx, must preserve
//
// We modify `ebp` to the new (clustered) count by patching the saved-ebp
// slot in the pushad frame, so popad restores the smaller value.
asm(R"(
.section .text,"ax"
.global RangeRingClusterTrampoline
RangeRingClusterTrampoline:
    pushad

    # ecx still holds the vector pointer; ebp still holds the original count.
    push ebp                          # arg2: count
    mov  eax, [ecx+4]                 # data start = vec->begin
    push eax                          # arg1: data
    call _ClusterRingPositions
    add  esp, 8

    # popad pop order: edi, esi, ebp, esp(skipped), ebx, edx, ecx, eax
    # so the saved-ebp slot lives at [esp+8] right now.
    mov  [esp+8], eax                 # write the clustered count into ebp's slot

    popad

    # Re-execute the displaced instruction `mov eax, ds:0x10a6438` (sWldMap)
    # so the original code at 0x7EF5E7 sees the same machine state it would
    # have without the hook.
    mov  eax, ds:0x10a6438

    jmp  0x007EF5E7
)");
