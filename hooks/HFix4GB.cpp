asm(
  // Reconstructed-source cross-check (Draiget/faf-re, audited 2026-09-29):
  // kPageOwnerMapBytes is used to allocate gPageOwnerByPage, while page-owner
  // reads/writes index it with (address >> 12) and no 3 GiB clamp. Its shipped
  // 0x3FF000 size therefore leaves the highest 0x400 page indices uncovered.
  //
  // Allocate one 4-byte map entry for every 4 KiB page in the complete
  // 32-bit virtual address space: 0x100000 entries * 4 = 0x400000 bytes.
  ".section h0; .set h0,0x957E35;"
  "PUSH 0x400000;"

  ".section h1; .set h1,0x915A92;"
  "ADD EAX,EAX;"
  "JGE .+0x73;"
  "MOV EAX,DWORD PTR DS:[ESI+0x2C];"
  "ADD EAX,0x10000000;"
  "JMP .+0x69;"

  ".section h2; .set h2,0x915B05;"
  "JMP .-0x73;"

  // The reconstructed allocator uses the shipped 0xC0000 constant only as
  // ReleaseHeapRecord's right-neighbour pageEnd guard. The left-neighbour path
  // has no 3 GiB boundary, and both merge paths still require the same
  // reservedBase. No other recovered allocator use gives 0xC0000 a separate
  // invariant. Keep 0x100000 as the one-past-end page index while allowing
  // valid free regions in the 3-4 GiB range to use the same right-neighbour
  // coalescing logic.
  ".section h3; .set h3,0x958107;"
  "CMP EAX,0x100000;"
);
