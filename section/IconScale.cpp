#include <magic_classes.h>

float iconscale = 1.0;
ConDescReg icons_scale_var{"ui_strategicIconScale", "", &iconscale};

void ScaleIcons()
{
    asm(
        "movaps  xmm0, xmm2;"
        "mulss xmm4, ds:%[iconscale];"
        "mulss xmm5, ds:%[iconscale];"
        "addss   xmm0, xmm4;"
        "jmp 0x85DE0C;"
        :
        : [iconscale] "i"(&iconscale)
        :);
}
