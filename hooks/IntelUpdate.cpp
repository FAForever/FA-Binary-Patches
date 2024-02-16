#include "../define.h"

asm(R"(

#Moho::CIntelPosHandle::UpdatePos at 0x76F1E0
# Intel handles update the intel raster grid when one of the following two
# conditions are met:
#   * The unit has moved more than a certain ratio of the intel radius beyond
#     the last update position
    radius_movement_ratio = 0xE4F6EC # 0.25 from 0.33
#   * a certain number of ticks have passed since the last update
    tick_update_interval = 5 # from 30


# We'll leave this alone for now.
#.section h0; .set h0,0x76F21A #Moho::CIntelPosHandle::UpdatePos+0x3A
#   fmul dword ptr [radius_movement_ratio]

.section h1; .set h1,0x76F258 #Moho::CIntelPosHandle::UpdatePos+0x78
    cmp edx, tick_update_interval

)");
