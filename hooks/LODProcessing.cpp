#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x005039F3)
    "jmp "QU(FirstLODCheck)";"   //initial max LODCutoff check
    "nop;"
    
    SECTION(2, 0x007DFBE6)
    "call "QU(ComputeLOD)";"
    
    SECTION(3, 0x007DFC0C)
    "movss xmm0, xmm7;"       //Taking our modified LODCutoff that we saved before in MeshComputeLOD
    "nop;"                    

    SECTION(4, 0x007DFC31)    //same
    "movss xmm0, xmm7;"       
    "nop;"
);