#include "include/moho.h"
void SetAlpha(int control, float alpha, uint32_t alphai)
{
    *(float *)(control + 240) = alpha;
    *(uint32_t *)(control + 244) = (*(uint32_t *)(control + 244) & 0x00FFFFFFu) | (alphai << 24);
}

int __thiscall _SetAlpha(int control, float alpha)
{
    uint32_t alphai = 0xFF * alpha;
    if (alphai > 0xFF)
    {
        alphai = 0xFF;
    }
    SetAlpha(control, alpha, alphai);
    return alphai;
}

int __fastcall NextChild(int child, int parent) asm("0x786EA0");

void __thiscall _SetAlphaChildren(int control, float alpha)
{
    uint32_t alphai = 0xFF * alpha;
    if (alphai > 0xFF)
    {
        alphai = 0xFF;
    }
    int child = control;
    do
    {
        SetAlpha(child, alpha, alphai);
        child = NextChild(child, control);
    } while (child);
}