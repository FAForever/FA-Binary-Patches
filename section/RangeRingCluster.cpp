#include "magic_classes.h"
#include "global.h"

// Hull culling for range rings.
//
// func_RenderRings (0x007EF5A0) gets a vector of N ring positions and
// renders every single one. With dense crowds (600+ units) the per-ring
// stencil fill cost dominates frame time.
//
// This patch hooks at 0x007EF5E2 and runs a greedy compaction pass that
// drops rings whose whole band is covered by already-kept rings. Interior
// rings get dropped; boundary and isolated rings survive. The engine's
// stencil-based outline rendering then produces the same merged hull from
// fewer rings.
//
// Disabled by default. Enable: `ui_RangeRingClusterHull 1`.

float g_RingClusterHull = 0.0f;

ConDescReg ring_cluster_hull_reg{
    "ui_RangeRingClusterHull",
    "If non-zero, cull range rings whose band is fully covered by "
    "already-kept neighbours (exact perimeter-interval test). Only boundary "
    "and isolated rings render. Large FPS gain in dense crowds.",
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
// === Greedy hull culling ===
//
// Candidates are processed in input order and tested ONLY against the
// already-kept set, compacting the vector in place:
//
//   * data[0 .. writeIdx)  is the keep set so far
//   * data[i]              is the candidate being inspected
//   * If P's band is covered by data[0..writeIdx), drop it
//   * Otherwise copy P to data[writeIdx] and bump writeIdx
//
// The greedy order guarantees every culled ring is covered by rings that
// survive in the final set, so the kept set's stencil mask equals the full
// original mask. Testing against "all others" instead would be unsound
// (two coincident rings would delete each other).
//
// === Exact coverage test ===
//
// An earlier revision sampled 24 points on the candidate's outer circle.
// That is structurally unable to certify area coverage: a candidate whose
// rim is covered by a necklace of smaller kept discs while its center
// region is uncovered gets dropped and leaves a hole in the stencil mask
// (raster-simulated: up to 6.4% of the union area lost; interstitial holes
// also appear in ordinary hex-packed formations; no fixed sample pattern
// closes all cases).
//
// The replacement is the exact perimeter-interval criterion (the
// redundancy form of the Huang-Tseng perimeter-coverage theorem, "The
// Coverage Problem in a Wireless Sensor Network", WSNA'03): any hole in
// the kept union is bounded by arcs of kept-ring boundary circles, so
// area coverage of P's band [rIn, rOut] holds if and only if
//
//   1. P's outer circle is fully covered by the union of kept bands,
//   2. P's inner circle (if rIn > 0) is fully covered likewise,
//   3. for every kept ring Q, the arcs of Q's outer/inner boundary circle
//      lying strictly inside P's band are covered by the union of the
//      OTHER kept bands (otherwise crossing that arc exits the union:
//      a hole).
//
// All three are 1D angular-interval checks: a kept band [a, b] covers, on
// a test circle of radius r around c, the angle set where
// d(theta)^2 = D^2 + r^2 - 2 D r cos(theta - phi) lies in [a^2, b^2] --
// up to two intervals from acos of the two bounds. Collect, sort, merge,
// check. Cost is O(m^2 log m) per candidate with m = kept rings in the
// 3x3 cell neighbourhood (tens); a single-keeper containment fast path
// (d + rOutP <= rOutQ, band-inclusive) catches stacked units first.
//
// Validated against a 2048px raster ground truth over 23 layout instances
// (hex/grid blobs, necklaces, mixed radii, random, stacks): 0 error pixels
// in all instances while keeping 98% of the sampling test's cull rate.
//
// === O(N) via spatial hash grid ===
//
// Naively testing each candidate against all kept rings is O(n^2) and made
// sparse crowds slower. Fix: spatial hash grid, stack/BSS allocated (no
// heap; build is -nostdlib). Cells are sized 2*maxOuter so any ring whose
// band could touch the candidate's disc has its center within the 3x3 cell
// neighbourhood. Sparse layouts find zero neighbours and keep immediately.
//
// Hash collisions are harmless: rings from unrelated cells get skipped by
// the distance check. Primes 73856093/19349663, 256 buckets (Teschner et
// al. 2003).

struct RangeRenderingParams
{
    float x;
    float z;
    float innerRadius;
    float outerRadius;
};

constexpr int HASH_BUCKETS = 256;
constexpr int HASH_MASK    = HASH_BUCKETS - 1;
constexpr int MAX_RINGS    = 4096; // engine has no ring limit (chunked dynamic VB); this only bounds our BSS
constexpr int MAX_LOCAL    = 192;  // kept rings considered per candidate; overflow -> keep candidate (safe)

constexpr float HC_PI  = 3.14159265f;
constexpr float HC_TAU = 6.28318531f;

// Angular slop: targets are shrunk and cover gaps glued by this much.
// 3e-4 rad on a radius-32 ring is a 1e-2 wu sliver -- far below one pixel
// at any zoom, and comfortably above both float32 rounding noise and the
// ~1e-5 rad error of the polynomial atan below.
constexpr float EPS_A = 3e-4f;
// World-unit slop: hole-check band shrink and closed-band comparisons
// (identical stacked rings land exactly on the band boundary in float).
constexpr float EPS_R = 1e-3f;

// Standard spatial hash (Teschner et al. 2003).
static inline unsigned hashCell(int cx, int cz)
{
    return (((unsigned)cx * 73856093u) ^ ((unsigned)cz * 19349663u)) & (unsigned)HASH_MASK;
}

// Polynomial atan2/acos: no libm dependency (build is -nostdlib; sqrtf
// maps to the engine's CRT at 0x452FC0 via global.h) and ~10x faster than
// x87 fpatan, which dominated the cull in dense blobs. Max error ~1e-5 rad
// (degree-11 odd minimax on [-1,1] + octant reduction), an order of
// magnitude below the EPS_A angular slop.
static float hc_atan_core(float z) // |z| <= 1
{
    float t = z * z;
    return z * (0.99997726f +
           t * (-0.33262347f +
           t * (0.19354346f +
           t * (-0.11643287f +
           t * (0.05265332f +
           t * (-0.01172120f))))));
}

static float hc_atan2(float y, float x)
{
    float ay = (y < 0.0f) ? -y : y;
    float ax = (x < 0.0f) ? -x : x;
    bool  swap = ay > ax;
    float den  = swap ? ay : ax;
    if (den < 1e-30f)
        return 0.0f;
    float a = hc_atan_core((swap ? ax : ay) / den);
    if (swap)
        a = 1.57079633f - a;
    if (x < 0.0f)
        a = HC_PI - a;
    return (y < 0.0f) ? -a : a;
}

static float hc_acos(float c)
{
    if (c >= 1.0f) return 0.0f;
    if (c <= -1.0f) return HC_PI;
    return hc_atan2(sqrtf(1.0f - c * c), c);
}

struct Iv
{
    float lo;
    float hi;
};

// Remaining-uncovered set of a coverage target: instead of accumulating all
// keeper intervals and sort-merging them (O(m^2 log m) per candidate, the
// dominant cost in dense blobs), keeper intervals are SUBTRACTED from the
// target as they are computed, with an early exit once nothing remains --
// in dense clusters that happens after the first few neighbours. Subtracted
// intervals are expanded by EPS_A/2 per side (equivalent to the gap-glue of
// the merge formulation); targets are shrunk by EPS_A.
constexpr int REM_CAP = 32;
struct Rem
{
    Iv  p[REM_CAP];
    int n;
};

static int s_local[MAX_LOCAL];

static float wrapTau(float x) // into [0, TAU)
{
    x -= HC_TAU * (float)(int)(x * (1.0f / HC_TAU));
    if (x < 0.0f) x += HC_TAU;
    if (x >= HC_TAU) x -= HC_TAU;
    return x;
}

// Append raw interval [lo, hi] (hi > lo, length <= TAU) as normalized
// [0,TAU)-pieces. Sets *full when it spans the whole circle.
static int addNorm(Iv *arr, int n, float lo, float hi, bool *full)
{
    float len = hi - lo;
    if (len <= 0.0f) return n;
    if (len >= HC_TAU - 1e-6f)
    {
        *full = true;
        return n;
    }
    lo = wrapTau(lo);
    hi = lo + len;
    if (hi > HC_TAU)
    {
        arr[n++] = {lo, HC_TAU};
        arr[n++] = {0.0f, hi - HC_TAU};
    }
    else
        arr[n++] = {lo, hi};
    return n;
}

// Angular intervals on the circle (center cx/cz, radius r) where the point
// lies inside the band a <= dist to (qx,qz) <= b. Appends normalized
// pieces to arr, returns the new count, sets *full for whole-circle
// coverage. d(theta)^2 = D^2 + r^2 - 2*D*r*cos(theta - phi).
static int bandIntervalsOnCircle(float cx, float cz, float r,
                                 float qx, float qz, float a, float b,
                                 Iv *arr, int n, bool *full)
{
    if (a < 0.0f) a = 0.0f;
    float dx = qx - cx;
    float dz = qz - cz;
    float D2 = dx * dx + dz * dz;
    // Cheap squared-form empty tests before any sqrt/atan. These are exactly
    // the |D-r| > b (ub > 1) and D+r < a (ua < -1) conditions below:
    {
        float s = r + b;
        if (D2 >= s * s) return n;          // circle entirely outside the band
        float t = r - b;
        if (t > 0.0f && D2 <= t * t) return n; // band entirely inside the circle
        float u = a - r;
        if (u > 0.0f && D2 <= u * u) return n; // circle entirely inside the hole
    }
    float D = sqrtf(D2);
    if (r < 1e-6f || D < 1e-6f)
    {
        // degenerate: concentric circles / point candidate
        float ref = (r < 1e-6f) ? D : r;
        if (a - EPS_R <= ref && ref <= b + EPS_R)
            *full = true;
        return n;
    }
    float phi   = hc_atan2(dz, dx);
    float denom = 2.0f * D * r;
    float ub = (D2 + r * r - b * b) / denom; // d <= b  <=>  cos(delta) >= ub
    float ua = (D2 + r * r - a * a) / denom; // d >= a  <=>  cos(delta) <= ua
    if (ub > 1.0f || ua < -1.0f)
        return n;
    float alpha = hc_acos(ub);                       // |delta| <= alpha
    float beta  = (ua >= 1.0f) ? 0.0f : hc_acos(ua); // |delta| >= beta
    if (alpha < beta)
        return n;
    if (beta <= 0.0f)
    {
        if (alpha >= HC_PI - 1e-6f)
        {
            *full = true;
            return n;
        }
        return addNorm(arr, n, phi - alpha, phi + alpha, full);
    }
    if (alpha >= HC_PI - 1e-6f)
        return addNorm(arr, n, phi + beta, phi + HC_TAU - beta, full);
    n = addNorm(arr, n, phi + beta, phi + alpha, full);
    n = addNorm(arr, n, phi - alpha, phi - beta, full);
    return n;
}

// Subtract normalized [a, b] (0 <= a < b <= TAU) from the remaining set.
// Returns false on fragment overflow (caller then keeps the ring: safe).
static bool remSubtractNorm(Rem &r, float a, float b)
{
    int n = r.n;
    for (int i = 0; i < n;)
    {
        float x = r.p[i].lo;
        float y = r.p[i].hi;
        if (b <= x || a >= y)
        {
            ++i;
        }
        else if (a <= x && b >= y)
        {
            r.p[i] = r.p[--n]; // fully covered: swap-delete, revisit slot i
        }
        else if (a > x && b < y)
        {
            if (n >= REM_CAP)
                return false;
            r.p[n++]  = {b, y};
            r.p[i].hi = a;
            ++i;
        }
        else if (a <= x)
        {
            r.p[i].lo = b;
            ++i;
        }
        else
        {
            r.p[i].hi = a;
            ++i;
        }
    }
    r.n = n;
    return true;
}

// Subtract a batch of normalized cover pieces, each expanded by EPS_A/2
// per side (the gap-glue equivalent). Sets *ok=false on overflow.
static void remSubtractPieces(Rem &r, const Iv *piece, int np, bool *ok)
{
    for (int i = 0; i < np && r.n > 0 && *ok; ++i)
    {
        float a = piece[i].lo - EPS_A * 0.5f;
        float b = piece[i].hi + EPS_A * 0.5f;
        if (a < 0.0f)
        {
            if (!remSubtractNorm(r, 0.0f, b)) { *ok = false; return; }
            if (!remSubtractNorm(r, a + HC_TAU, HC_TAU)) { *ok = false; return; }
        }
        else if (b > HC_TAU)
        {
            if (!remSubtractNorm(r, a, HC_TAU)) { *ok = false; return; }
            if (!remSubtractNorm(r, 0.0f, b - HC_TAU)) { *ok = false; return; }
        }
        else if (!remSubtractNorm(r, a, b))
        {
            *ok = false;
            return;
        }
    }
}

// Coverage of a full test circle (center cx/cz, radius r) by the union of
// the m local keeper bands, with subtract-and-early-exit.
static bool circleCovered(const RangeRenderingParams *data, const int *loc, int m,
                          float cx, float cz, float r)
{
    Rem rem;
    rem.p[0] = {EPS_A, HC_TAU - EPS_A};
    rem.n    = 1;
    bool ok  = true;
    Iv   tmp[4];
    for (int k = 0; k < m && rem.n > 0; ++k)
    {
        const RangeRenderingParams &Q = data[loc[k]];
        bool full = false;
        int  tn   = bandIntervalsOnCircle(cx, cz, r,
                                          Q.x, Q.z, Q.innerRadius, Q.outerRadius,
                                          tmp, 0, &full);
        if (full)
            return true;
        remSubtractPieces(rem, tmp, tn, &ok);
        if (!ok)
            return false; // fragment overflow: keep the ring, stay safe
    }
    return rem.n == 0;
}

// Exact band-coverage test of candidate P against the m local keepers.
static bool exactCovered(const RangeRenderingParams *data, const RangeRenderingParams &P,
                         const int *loc, int m)
{
    // 0) Cheap keep-shortcut (sound in the keep direction only): if any of
    //    8 rim samples lies clearly outside every keeper band -- by more
    //    than the slop the exact sweeps would forgive -- the rim cannot be
    //    fully covered and the sweeps are skipped. Kept candidates are the
    //    expensive path of the sweeps (no early exit), so this pays off in
    //    blob boundaries and loose formations.
    static const float DIR8X[8] = {1.0f, 0.70710678f, 0.0f, -0.70710678f,
                                   -1.0f, -0.70710678f, 0.0f, 0.70710678f};
    static const float DIR8Z[8] = {0.0f, 0.70710678f, 1.0f, 0.70710678f,
                                   0.0f, -0.70710678f, -1.0f, -0.70710678f};
    float slack = EPS_R + P.outerRadius * EPS_A;
    for (int s = 0; s < 8; ++s)
    {
        float sx = P.x + P.outerRadius * DIR8X[s];
        float sz = P.z + P.outerRadius * DIR8Z[s];
        bool  covered = false;
        for (int k = 0; k < m; ++k)
        {
            const RangeRenderingParams &Q = data[loc[k]];
            float dx = sx - Q.x;
            float dz = sz - Q.z;
            float d2 = dx * dx + dz * dz;
            float qo = Q.outerRadius + slack;
            if (d2 > qo * qo)
                continue;
            float qi = Q.innerRadius - slack;
            if (qi > 0.0f && d2 < qi * qi)
                continue;
            covered = true;
            break;
        }
        if (!covered)
            return false;
    }

    // 1) P's outer circle fully covered by the union of kept bands
    if (!circleCovered(data, loc, m, P.x, P.z, P.outerRadius))
        return false;

    // 2) P's inner circle (band's inner boundary), if present
    if (P.innerRadius > 0.0f &&
        !circleCovered(data, loc, m, P.x, P.z, P.innerRadius))
        return false;

    // 3) hole check: every kept boundary arc strictly inside P's band must
    //    be covered by the union of the OTHER kept bands
    float aIn = (P.innerRadius > 0.0f) ? P.innerRadius + EPS_R : 0.0f;
    float bIn = P.outerRadius - EPS_R;
    if (bIn <= aIn)
        return true;
    Iv tmp[4];
    for (int j = 0; j < m; ++j)
    {
        const RangeRenderingParams &Q = data[loc[j]];
        for (int c = 0; c < 2; ++c)
        {
            float cr = c ? Q.innerRadius : Q.outerRadius;
            if (cr <= 0.0f)
                continue;
            bool arcFull = false;
            int  an      = 0;
            an = bandIntervalsOnCircle(Q.x, Q.z, cr, P.x, P.z, aIn, bIn,
                                       tmp, an, &arcFull);
            Rem rem;
            if (arcFull)
            {
                rem.p[0] = {EPS_A, HC_TAU - EPS_A};
                rem.n    = 1;
            }
            else
            {
                rem.n = 0;
                for (int t = 0; t < an; ++t)
                {
                    float lo = tmp[t].lo + EPS_A;
                    float hi = tmp[t].hi - EPS_A;
                    if (hi > lo)
                        rem.p[rem.n++] = {lo, hi};
                }
                if (rem.n == 0)
                    continue;
            }
            bool ok = true;
            for (int l = 0; l < m && rem.n > 0; ++l)
            {
                if (l == j)
                    continue;
                const RangeRenderingParams &K = data[loc[l]];
                bool covFull = false;
                int  cn      = bandIntervalsOnCircle(Q.x, Q.z, cr,
                                                     K.x, K.z, K.innerRadius, K.outerRadius,
                                                     tmp, 0, &covFull);
                if (covFull)
                {
                    rem.n = 0;
                    break;
                }
                remSubtractPieces(rem, tmp, cn, &ok);
                if (!ok)
                    return false; // fragment overflow: keep the ring
            }
            if (rem.n > 0)
                return false;
        }
    }
    return true;
}

extern "C" int ClusterRingPositions(RangeRenderingParams *data, size_t count)
{
    if (g_RingClusterHull == 0.0f || count <= 4)
        return count;

    // Safety fallback: skip cull if input exceeds our BSS chain capacity.
    // Correctness preserved (renderer just sees the original list).
    if (count > (size_t)MAX_RINGS)
        return (int)count;

    // Cell size 2*maxOuter: any ring whose band can touch the candidate's
    // disc has its center within the 3x3 cell neighbourhood.
    float maxOuter = 0.0f;
    for (size_t i = 0; i < count; ++i)
        if (data[i].outerRadius > maxOuter) maxOuter = data[i].outerRadius;
    if (maxOuter <= 0.0f)
        return (int)count;

    const float invCellSize = 0.5f / maxOuter; // 1 / (2*maxOuter)

    // Bias so all map coordinates become positive before truncation,
    // avoiding the (int)cast-toward-zero asymmetry without floorf.
    constexpr float COORD_BIAS    = 1.0e6f;
    const int       BIAS_CELL_SUB = (int)(COORD_BIAS * invCellSize);

    // BSS grid: gridHead[h] = -1 empty; gridNext chains kept-ring indices.
    static int gridHead[HASH_BUCKETS];
    static int gridNext[MAX_RINGS];
    for (int i = 0; i < HASH_BUCKETS; ++i) gridHead[i] = -1;

    int writeIdx = 0;

    for (size_t i = 0; i < count; ++i)
    {
        // Snapshot the candidate so it survives the in-place overwrite.
        const float px = data[i].x;
        const float pz = data[i].z;
        const float pInner = data[i].innerRadius;
        const float pOuter = data[i].outerRadius;

        const int pcx = (int)((px + COORD_BIAS) * invCellSize) - BIAS_CELL_SUB;
        const int pcz = (int)((pz + COORD_BIAS) * invCellSize) - BIAS_CELL_SUB;

        // Gather kept rings whose band can touch P's disc (3x3 cells).
        int  m        = 0;
        bool overflow = false;
        for (int dx = -1; dx <= 1 && !overflow; ++dx)
        {
            for (int dz = -1; dz <= 1 && !overflow; ++dz)
            {
                unsigned h = hashCell(pcx + dx, pcz + dz);
                for (int j = gridHead[h]; j >= 0; j = gridNext[j])
                {
                    float ddx   = data[j].x - px;
                    float ddz   = data[j].z - pz;
                    float reach = pOuter + data[j].outerRadius + EPS_R;
                    if (ddx * ddx + ddz * ddz <= reach * reach)
                    {
                        if (m >= MAX_LOCAL)
                        {
                            overflow = true; // too crowded: keep P, stay safe
                            break;
                        }
                        s_local[m++] = j;
                    }
                }
            }
        }

        bool drop = false;
        if (!overflow && m > 0)
        {
            // Fast path: a single keeper containing P's whole band
            // (d + rOutP <= rOutQ and Q's hole inside P's hole). Catches
            // stacked/teleported units without any interval work.
            for (int k = 0; k < m; ++k)
            {
                const RangeRenderingParams &Q = data[s_local[k]];
                float ddx = Q.x - px;
                float ddz = Q.z - pz;
                float d   = sqrtf(ddx * ddx + ddz * ddz);
                if (d + pOuter <= Q.outerRadius + EPS_R &&
                    (Q.innerRadius <= 0.0f || d + Q.innerRadius <= pInner + EPS_R))
                {
                    drop = true;
                    break;
                }
            }
            if (!drop)
            {
                RangeRenderingParams P = {px, pz, pInner, pOuter};
                drop = exactCovered(data, P, s_local, m);
            }
        }

        if (!drop)
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
