#include "magic_classes.h"

// Hide lifebars on full-HP units.
//
// The lifebar render path in Moho::CWldSession::RenderStrategicIcons
// enqueues a unit lifebar via sub_85EED0 (0x85EED0) for every visible
// non-dead unit. With hundreds of undamaged units crowded in one spot the
// per-frame batching cost dominates the renderer. This patch gates the
// enqueue on the unit's HP: when `ui_LifebarOnlyDamaged` is set, lifebars
// only draw for units whose mHealth < mMaxHealth (= took any damage).
//
// Default is 0 (vanilla behaviour). Set to 1 to enable the filter.
//
// HP/MaxHealth offsets in Moho::UserEntity verified via IDA struct DB:
//   UserEntity_base.mVarData              @ +0x4C
//   SSTIEntityVariableData.mHealth        @ +0x18  -> UserEntity +0x64
//   SSTIEntityVariableData.mMaxHealth     @ +0x1C  -> UserEntity +0x68
//
// File-scope C++ globals get the mingw underscore prefix (_g_...) but are
// NOT C++-mangled, so they can be referenced directly from the asm below.

bool g_LifebarOnlyDamaged = false;

ConDescReg lf_only_damaged_reg{
    "ui_LifebarOnlyDamaged",
    "If 1, hide lifebars for units at full HP -- they only appear once a "
    "unit takes damage. Default 0 (vanilla: always show). Big FPS gain in "
    "dense crowds where most units are undamaged.",
    &g_LifebarOnlyDamaged};

// Custom calling convention inherited from sub_85EED0 (callcnv_F3):
//   eax = lifebar entry pointer (a 64-byte stack buffer prepared by
//         RenderStrategicIcons; sub_85F140 inside sub_85EED0 memcpy's it
//         into the batch vector)
//   ecx = std::vector<...>* (lifebar batch destination)
//
// HP/MaxHealth offsets discovered by reading sub_85CD40 (the function
// that draws each lifebar from the batch vector). It computes the bar
// fill fraction as:
//
//     v18 = *(float **)eax0;            // entry[0] = unit data pointer
//     v19 = v18[26] / v18[27];          // current / max
//
// So the unit's current HP and max HP live at offsets 104 and 108 from
// the pointer stored at entry[0]. We read the same fields the renderer
// reads -- whatever struct *entry[0] points to, the renderer treats
// offsets 104/108 as the HP pair.
//
// Inside our filter, after PUSHAD the original eax (the entry pointer)
// is at [esp+28] (pushad order: edi, esi, ebp, esp, ebx, edx, ecx, eax).
//
// On suppress: popad + ret (rewinds the call). On render: popad + jmp
// to the original sub_85EED0 (tail-call preserves the eax/ecx args).
asm(R"(
.section .text,"ax"
.global LifebarFilter
LifebarFilter:
    pushad

    # Filter disabled? -> render normally.
    mov     dl, ds:_g_LifebarOnlyDamaged
    test    dl, dl
    jz      .L_lf_render

    # Recover the entry pointer (saved eax slot in pushad frame).
    mov     edx, [esp+28]
    test    edx, edx
    jz      .L_lf_render

    # entry[0] -> pointer to unit data struct
    mov     edx, [edx]
    test    edx, edx
    jz      .L_lf_render

    # Read the same HP pair sub_85CD40 uses for the bar fill fraction.
    movss   xmm0, [edx+104]    # current HP   (v18[26])
    movss   xmm1, [edx+108]    # max HP       (v18[27])

    # Render iff current < max (unit took damage).
    comiss  xmm1, xmm0
    ja      .L_lf_render

    # Suppress: restore regs and return without calling sub_85EED0.
    popad
    ret

.L_lf_render:
    # Tail-call the original sub_85EED0 with the original eax/ecx.
    popad
    jmp     0x85EED0
)");
