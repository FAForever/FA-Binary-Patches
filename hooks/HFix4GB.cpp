asm(
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

  // Keep 0x100000 as the one-past-end page index while allowing free
  // regions in the 3-4 GiB range to coalesce with a right neighbour.
  ".section h3; .set h3,0x958107;"
  "CMP EAX,0x100000;"
);
