asm(
  // The constructor at 0x4928A0 builds a fixed pool of particle
  // render-buffer descriptors. Keep each descriptor's native capacity
  // unchanged and raise only the number of available descriptors.
  ".section h0; .set h0,0x4928ED;"
  "MOV DWORD PTR [ESP+0x10],0x1000;"

  // The same constructor builds a separate fixed pool used by segment
  // buffers. The native per-buffer creation arguments remain unchanged.
  ".section h1; .set h1,0x4929B2;"
  "MOV DWORD PTR [ESP+0x10],0x190;"
);
