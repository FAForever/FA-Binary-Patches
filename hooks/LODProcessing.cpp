#define SECTION(index, address) ".section h"#index"; .set h"#index","#address";"
#include "../define.h"
asm(
    
    SECTION(1, 0x005039F3)
    "jmp "QU(FirstLODCheck)";"   //initial max LODCutoff check
    "nop;"
    
    SECTION(2, 0x007DDA73)
    "jmp "QU(MeshComputeLOD)";"  //part of Moho::Mesh::ComputeLOD
    
    SECTION(3, 0x007DDA83)
    "jmp "QU(ProcessBestLOD)";"  //part of Moho::Mesh::ComputeLOD
    
    SECTION(4, 0x007DDA9B)
    "jmp "QU(ProcessWorstLOD)";" //part of Moho::Mesh::ComputeLOD
    "nop;"
    "nop;"
    "nop;"
    
    SECTION(5, 0x007DFC0C)
    "movss xmm1, xmm7;"       //Taking our modified LODCutoff that we saved before in MeshComputeLOD
    "nop;"                    

    SECTION(6, 0x007DFC31)    //same
    "movss xmm1, xmm7;"       
    "nop;"
);