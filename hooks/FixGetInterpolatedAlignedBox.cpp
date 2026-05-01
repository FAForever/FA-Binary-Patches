

asm(R"(
.section h0; .set h0,0x086D2C0 #cfunc_CUIWorldMeshGetInterpolatedAlignedBox+0x150
    push offset )" QU(strYMax) R"( # "yMax" from "xMax"

.section h1; .set h1,0x086D2D6 #cfunc_CUIWorldMeshGetInterpolatedAlignedBox+0x166
    push offset )" QU(strZMax) R"( # "zMax" from "xMax"
)");