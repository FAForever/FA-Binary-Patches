#include "magic_classes.h"

SHARED bool render_minimap_ranges = true;
ConDescReg<bool> ui_RenderMinimapRanges_var("ui_RenderMinimapRanges", "", &render_minimap_ranges);
