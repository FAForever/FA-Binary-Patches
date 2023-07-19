#include "../define.h"
asm(
  ".section h0; .set h0,0x6CE3BA;"
  "CALL "QU(SimGetCommandQueueInsert)";"
  );
