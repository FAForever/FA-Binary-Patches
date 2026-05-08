#include "magic_classes.h"

SHARED bool render_minimap_ranges = true;
ConDescReg<bool> ren_MinimapRanges_var("ren_MinimapRanges", "", &render_minimap_ranges);
