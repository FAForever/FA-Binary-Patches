#include "../define.h"
asm(
  //HOOK SiloAmmoCheckArgs
  ".section h0; .set h0,0x6CED77;"
  "JMP "QU(SiloAmmoCheckArgs)";"
  
  //HOOK SiloAmmoSetBlocks
  ".section h1; .set h1,0x6CEDF1;"
  "JMP "QU(SiloAmmoSetBlocks)";"
);