#include "desync_fix_global.h"

// Intermediate RangeMask flush to prevent 7-bit stencil overflow
// in func_RenderRings (0x7EF5A0) first Cast loop.
//
// Calling conventions:
//   InitTransformedVerts (0x7F5DA0): this@<ebx>, (float w, float h), retn 8
//   AssignString         (0x4059E0): this@<ecx>, (char* s, size_t n), retn 8
//   CRenFrame::Render    (0x7F6030): this@<edi>, (int w, int h), retn 8
//
// Replaces 7-byte instruction at 0x7EF7EA: mov eax, [esp+0x84]

void IntermediateRangeMask()
{
	asm(
		"pushad;"
		// ESP offset: +4 (call ret) +32 (pushad) = +36
		"mov esi, dword ptr [esp+0x9C];"  // Head*  [0x78+36]
		"mov edi, dword ptr [esp+0x98];"  // arg_0  [0x74+36]
		"add edi, 0x4C;"                   // technique ptr

		"cvtsi2ss xmm0, dword ptr [esi+0x14];"
		"sub esp, 8;"
		"movss dword ptr [esp+4], xmm0;"
		"cvtsi2ss xmm0, dword ptr [esi+0x10];"
		"mov ebx, edi;"
		"movss dword ptr [esp], xmm0;"
	);
	asm(
		"call -0x1000 +8351136;"  // InitTransformedVerts
		"push 9;"
		"push 0xE3F8E8;"         // "RangeMask"
		"mov ecx, edi;"
	);
	asm(
		"call -0x1000 +4221408;"  // AssignString
		"mov ecx, dword ptr [esi+0x14];"
		"mov edx, dword ptr [esi+0x10];"
		"push ecx;"
		"push edx;"
	);
	asm(
		"call -0x1000 +8351792;"  // CRenFrame::Render
		"popad;"
		"mov eax, dword ptr [esp+0x88];"
		"ret;"
	);
}
