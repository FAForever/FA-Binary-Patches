#include "include/magic_classes.h"

float iconscale = 1.0;
ConDescReg icons_scale_var{"ui_StrategicIconScale", "", &iconscale};

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
void ScalePauseIcon()
{
    asm(
        "movaps  xmm0, xmm2;"
        "mulss xmm4, ds:%[iconscale];"
        "mulss xmm5, ds:%[iconscale];"
        "addss   xmm0, xmm4;"
        "jmp 0x85DF09;"
        :
        : [iconscale] "i"(&iconscale)
        :);
}

void ScaleStunIcon()
{
    asm(
        "movaps  xmm0, xmm3;"
        "movaps  xmm1, xmm2;"
        "mulss xmm5, ds:%[iconscale];"
        "mulss xmm6, ds:%[iconscale];"
        "jmp 0x85E009;"
        :
        : [iconscale] "i"(&iconscale)
        :);
}

void ScaleUnderlay()
{
    asm(
        "movaps  xmm1, xmm6;"
        "mulss xmm5, ds:%[iconscale];"
        "mulss xmm4, ds:%[iconscale];"
        "addss   xmm1, xmm5;"
        "jmp 0x85DCFE;"
        :
        : [iconscale] "i"(&iconscale)
        :);
}

