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
// === O(N) via spatial hash grid ===
//
// The naive nested loop over all writeIdx kept rings is O(n²): for sparse
// crowds (600 rings spread across the map, no overlap) writeIdx grows to N
// and the inner loop runs 24×N×N times producing zero culls — the cure was
// worse than the disease (140→70 FPS regression on real testing).
//
// Fix: spatial hash grid. Stack-allocated, no heap (build is -nostdlib so
// memmove/memset/new are unavailable). Cells are sized at 2×maxOuter so any
// ring whose circle could overlap P's circle has its center within the 3×3
// cell neighbourhood of P's cell. This makes both the pre-check and the
// 24-sample test O(K_local) per query where K_local = rings in 9 cells,
// independent of N.
//
//   Sparse case (worst with naive algo): pre-check finds zero rings in 3×3,
//     keeps P immediately, no 24-sample loop. Total: ~O(N).
//   Dense case: writeIdx stays small from culling; grid lookup costs the
//     same O(K_local) but with K_local ≈ writeIdx anyway. Same as before.
//
// Hash collisions are harmless: rings from unrelated cells just get skipped
// via the actual distance check. With primes 73856093/19349663 and 256
// buckets, expected chain length for 1000 rings is ~4 (well-distributed).

struct RangeRenderingParams
{
    float x;
    float z;
    float innerRadius;
    float outerRadius;
};

// Spatial hash parameters.
// HASH_BUCKETS power-of-two for bitmask, MAX_RINGS bounds the stack arrays.
constexpr int HASH_BUCKETS = 256;
constexpr int HASH_MASK    = HASH_BUCKETS - 1;
constexpr int MAX_RINGS    = 1024;

// Standard spatial hash (Teschner et al. 2003).
static inline unsigned hashCell(int cx, int cz)
{
    return (((unsigned)cx * 73856093u) ^ ((unsigned)cz * 19349663u)) & (unsigned)HASH_MASK;
}

extern "C" int ClusterRingPositions(RangeRenderingParams *data, size_t count)
{
    if (g_RingClusterHull == 0.0f || count <= 4)
        return count;

    // Safety fallback: skip cull if input exceeds our stack-allocated chain
    // capacity. Rare in practice; correctness preserved (renderer just sees
    // the original list).
    if (count > (size_t)MAX_RINGS)
        return (int)count;

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

    // Pre-pass: maximum outer radius determines cell size. With cell size
    // 2*maxOuter, any covering ring's center lies within ±1 cell of the
    // candidate's cell, so the lookup region is always 3×3 cells.
    float maxOuter = 0.0f;
    for (size_t i = 0; i < count; ++i)
        if (data[i].outerRadius > maxOuter) maxOuter = data[i].outerRadius;
    if (maxOuter <= 0.0f)
        return (int)count;

    const float invCellSize = 0.5f / maxOuter; // 1 / (2*maxOuter)

    // Bias offset so all map coordinates become positive before truncation,
    // avoiding the C++ (int)cast-toward-zero asymmetry without calling
    // floorf (libc may not be linked).
    constexpr float COORD_BIAS    = 1.0e6f;
    const int       BIAS_CELL_SUB = (int)(COORD_BIAS * invCellSize);

    // BSS-allocated grid. gridHead[h] = -1 means empty bucket. gridNext[i]
    // chains kept-ring indices that share a hash bucket. Static storage
    // avoids stack-probe (__chkstk_ms) calls — GCC inserts those for any
    // function with >4 KB local arrays, and that symbol isn't linked here.
    // Render is single-threaded so the shared static is safe.
    static int gridHead[HASH_BUCKETS];
    static int gridNext[MAX_RINGS];
    for (int i = 0; i < HASH_BUCKETS; ++i) gridHead[i] = -1;

    int writeIdx = 0;

    for (size_t i = 0; i < count; ++i)
    {
        // Snapshot the candidate so it survives any in-place overwrite.
        const float px = data[i].x;
        const float pz = data[i].z;
        const float pInner = data[i].innerRadius;
        const float pOuter = data[i].outerRadius;

        const int pcx = (int)((px + COORD_BIAS) * invCellSize) - BIAS_CELL_SUB;
        const int pcz = (int)((pz + COORD_BIAS) * invCellSize) - BIAS_CELL_SUB;

        // Pre-check: any kept ring near enough to possibly cover P?
        // A ring Q can cover any point on P's outer circle only if
        // dist(P.center, Q.center) <= pOuter + qOuter. Since cellSize is
        // 2*maxOuter >= pOuter + qOuter, the search region is 3×3 cells.
        bool any_close = false;
        for (int dx = -1; dx <= 1 && !any_close; ++dx)
        {
            for (int dz = -1; dz <= 1 && !any_close; ++dz)
            {
                unsigned h = hashCell(pcx + dx, pcz + dz);
                for (int j = gridHead[h]; j >= 0 && !any_close; j = gridNext[j])
                {
                    float ddx = data[j].x - px;
                    float ddz = data[j].z - pz;
                    float reach = pOuter + data[j].outerRadius;
                    if (ddx * ddx + ddz * ddz <= reach * reach)
                        any_close = true;
                }
            }
        }

        if (!any_close)
        {
            // No kept ring can possibly cover P -- keep immediately,
            // skipping the 24-sample loop entirely. This is the path
            // hit by sparse/spread-out crowds.
            data[writeIdx].x           = px;
            data[writeIdx].z           = pz;
            data[writeIdx].innerRadius = pInner;
            data[writeIdx].outerRadius = pOuter;
            unsigned h = hashCell(pcx, pcz);
            gridNext[writeIdx] = gridHead[h];
            gridHead[h] = writeIdx;
            ++writeIdx;
            continue;
        }

        // Full 24-sample coverage test. We only need to query rings whose
        // centers lie in the candidate's 3×3 neighbourhood (same bound as
        // above) -- a sample point is at distance pOuter from P, and a
        // covering ring's reach is at most maxOuter, total <= cellSize.
        bool fully_covered = true;
        for (int k = 0; k < 24 && fully_covered; ++k)
        {
            float tx = px + pOuter * COSDIR[k];
            float tz = pz + pOuter * SINDIR[k];

            bool sample_covered = false;
            for (int dx = -1; dx <= 1 && !sample_covered; ++dx)
            {
                for (int dz = -1; dz <= 1 && !sample_covered; ++dz)
                {
                    unsigned h = hashCell(pcx + dx, pcz + dz);
                    for (int j = gridHead[h]; j >= 0 && !sample_covered; j = gridNext[j])
                    {
                        float ddx = data[j].x - tx;
                        float ddz = data[j].z - tz;
                        float distSq = ddx * ddx + ddz * ddz;
                        float qOuter = data[j].outerRadius;
                        if (distSq > qOuter * qOuter) continue;
                        float qInner = data[j].innerRadius;
                        if (qInner > 0.0f && distSq < qInner * qInner) continue;
                        sample_covered = true;
                    }
                }
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
            unsigned h = hashCell(pcx, pcz);
            gridNext[writeIdx] = gridHead[h];
            gridHead[h] = writeIdx;
            ++writeIdx;
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
    pushfd                            # save EFLAGS so the engine at 0x7EF5E7
                                      # observes the same flags it would
                                      # without the hook

    # ecx still holds the vector pointer; ebp still holds the original count.
    push ebp                          # arg2: count
    mov  eax, [ecx+4]                 # data start = vec->begin
    push eax                          # arg1: data
    call _ClusterRingPositions
    add  esp, 8

    # Stack now: [pushfd dword][pushad frame: edi,esi,ebp,esp,ebx,edx,ecx,eax]
    # pushad pop order on popad: edi, esi, ebp, esp(skipped), ebx, edx, ecx, eax
    # so the saved-ebp slot is at offset 4 (pushfd) + 8 (edi+esi) = 12.
    mov  [esp+12], eax                # write the clustered count into ebp's slot

    popfd                             # restore EFLAGS
    popad

    # Re-execute the displaced instruction `mov eax, ds:0x10a6438` (sWldMap)
    # so the original code at 0x7EF5E7 sees the same machine state it would
    # have without the hook.
    mov  eax, ds:0x10a6438

    jmp  0x007EF5E7
)");
