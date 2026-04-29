#include "magic_classes.h"
#include <algorithm>

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
    "already-kept neighbours (greedy 24-sample geometric test). Only boundary "
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
//
// === O(n log n) spatial acceleration ===
//
// The naive inner loop over all writeIdx kept rings is O(n²). The fix:
//
//   1. Sort input by X once (O(n log n)).
//   2. Pre-check: before running the 24 samples, scan kept rings in reverse
//      (highest X first) and break when a ring's right edge falls left of the
//      candidate center. If no ring is within (pOuter + qOuter) range, keep
//      the candidate immediately — no samples needed. This eliminates the
//      O(n²) cost for sparse / spread-out scenarios.
//   3. Full sample loop: same reverse scan with X-cutoff break. Each sample
//      only checks rings whose X range overlaps the sample point.
//
// Complexity: O(n log n) sort + O(n × K_local × 24) for the coverage tests,
// where K_local = number of kept rings near each candidate. For sparse rings
// K_local ≈ 0 (pre-check short-circuits); for dense clusters writeIdx stays
// small due to culling. Worst-case is O(n log n) in all practical scenarios.

struct RangeRenderingParams
{
    float x;
    float z;
    float innerRadius;
    float outerRadius;
};

extern "C" int ClusterRingPositions(RangeRenderingParams *data, size_t count)
{
    if (g_RingClusterHull == 0.0f || count <= 4)
        return count;

    // 24 unit vectors at 15-degree intervals around the outer circle.
    // The engine draws rings as 45 segments (8 deg each); 24 samples at
    // 15 deg catches gaps down to ~2 segments, reducing outline choppiness.
    static const float COSDIR[24] = {
        1.00000000f, 0.96592583f, 0.86602540f, 0.70710677f,
        0.50000000f, 0.25881905f, 0.00000000f, -0.25881905f,
        -0.50000000f, -0.70710677f, -0.86602540f, -0.96592583f,
        -1.00000000f, -0.96592583f, -0.86602540f, -0.70710677f,
        -0.50000000f, -0.25881905f, 0.00000000f, 0.25881905f,
        0.50000000f, 0.70710677f, 0.86602540f, 0.96592583f};
    static const float SINDIR[24] = {
        0.00000000f, 0.25881905f, 0.50000000f, 0.70710677f,
        0.86602540f, 0.96592583f, 1.00000000f, 0.96592583f,
        0.86602540f, 0.70710677f, 0.50000000f, 0.25881905f,
        0.00000000f, -0.25881905f, -0.50000000f, -0.70710677f,
        -0.86602540f, -0.96592583f, -1.00000000f, -0.96592583f,
        -0.86602540f, -0.70710677f, -0.50000000f, -0.25881905f};

    // Sort by X so the inner scan can break early (O(n log n)).
    std::sort(data, data + count, [](const RangeRenderingParams &a, const RangeRenderingParams &b) {
        return a.x < b.x;
    });

    int writeIdx = 0;
    float maxKeptR = 0.0f; // running max outerRadius of the kept set

    for (int i = 0; i < (int)count; ++i)
    {
        // Snapshot the candidate so it survives in-place overwrites.
        float px    = data[i].x;
        float pz    = data[i].z;
        float pInner = data[i].innerRadius;
        float pOuter = data[i].outerRadius;

        // Pre-check: find any kept ring close enough to possibly cover P.
        // A ring Q can cover any point on P's outer circle only if
        //   dist(P.center, Q.center) ≤ pOuter + qOuter.
        // Scan backwards (high-X first); break when Q is too far left.
        bool any_close = false;
        float band = pOuter + maxKeptR;
        for (int j = writeIdx - 1; j >= 0 && !any_close; --j)
        {
            float dx = data[j].x - px;
            if (-dx > band) break;          // Q is too far left — and all j' < j are further left
            if (dx > band) continue;        // Q is too far right (X sorted, rare at start)
            float dz = data[j].z - pz;
            float reach = pOuter + data[j].outerRadius;
            if (dx * dx + dz * dz <= reach * reach)
                any_close = true;
        }

        if (!any_close)
        {
            // No kept ring can cover P — keep it immediately without 24 samples.
            data[writeIdx].x           = px;
            data[writeIdx].z           = pz;
            data[writeIdx].innerRadius = pInner;
            data[writeIdx].outerRadius = pOuter;
            ++writeIdx;
            if (pOuter > maxKeptR) maxKeptR = pOuter;
            continue;
        }

        // Full 24-sample coverage test — only reached when neighbours exist.
        bool fully_covered = true;
        for (int k = 0; k < 24 && fully_covered; ++k)
        {
            float tx = px + pOuter * COSDIR[k];
            float tz = pz + pOuter * SINDIR[k];

            bool sample_covered = false;
            for (int j = writeIdx - 1; j >= 0 && !sample_covered; --j)
            {
                float dx = data[j].x - tx;
                if (-dx > data[j].outerRadius) break; // Q too far left — break
                float dz = data[j].z - tz;
                float distSq = dx * dx + dz * dz;
                float qOuter = data[j].outerRadius;
                if (distSq > qOuter * qOuter) continue;
                float qInner = data[j].innerRadius;
                if (qInner > 0.0f && distSq < qInner * qInner) continue;
                sample_covered = true;
            }

            if (!sample_covered)
                fully_covered = false;
        }

        if (!fully_covered)
        {
            data[writeIdx].x           = px;
            data[writeIdx].z           = pz;
            data[writeIdx].innerRadius = pInner;
            data[writeIdx].outerRadius = pOuter;
            ++writeIdx;
            if (pOuter > maxKeptR) maxKeptR = pOuter;
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
    pushfd                            # save EFLAGS (engine at 0x7EF5E7 may read flags)

    # ecx still holds the vector pointer; ebp still holds the original count.
    push ebp                          # arg2: count
    mov  eax, [ecx+4]                 # data start = vec->begin
    push eax                          # arg1: data
    call _ClusterRingPositions
    add  esp, 8

    # Stack after call+add: [pushfd][pushad frame: edi,esi,ebp,esp,ebx,edx,ecx,eax]
    # pushfd is at [esp+0], pushad frame starts at [esp+4].
    # popad pop order: edi, esi, ebp(+8 from pushfd base), ...
    # saved-ebp slot is at [esp+4+8] = [esp+12].
    mov  [esp+12], eax                # write the clustered count into ebp's slot

    popfd                             # restore EFLAGS
    popad

    # Re-execute the displaced instruction `mov eax, ds:0x10a6438` (sWldMap)
    # so the original code at 0x7EF5E7 sees the same machine state it would
    # have without the hook.
    mov  eax, ds:0x10a6438

    jmp  0x007EF5E7
)");
