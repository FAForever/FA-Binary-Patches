
asm(
  //HOOK Check args
  ".section h0; .set h0,0x6CED74;"
  "JMP SiloAmmoCheckArgs;"

  //HOOK Set blocks
  ".section h1; .set h1,0x6CEDF1;"
  "JMP SiloAmmoSetBlocks;"
);