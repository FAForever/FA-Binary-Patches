#include "include/moho.h"
void SetAlpha(int bitmap, float alpha, uint32_t alphai)
{
    *(float *)(bitmap + 240) = alpha;
    *(uint32_t *)(bitmap + 244) = (*(uint32_t *)(bitmap + 244) & 0x00FFFFFFu) | (alphai << 24);
}

int __thiscall _SetAlpha(int bitmap, float alpha)
{
    uint32_t alphai = 0xFF * alpha;
    if (alphai > 0xFF)
    {
        alphai = 0xFF;
    }
    SetAlpha(bitmap, alpha, alphai);
    return alphai;
}

int __fastcall NextChild(int child, int parent) asm("0x786EA0");

void __thiscall _SetAlphaChildren(int bitmap, float alpha)
{
    uint32_t alphai = 0xFF * alpha;
    if (alphai > 0xFF)
    {
        alphai = 0xFF;
    }
    int child = bitmap;
    do
    {
        SetAlpha(child, alpha, alphai);
        child = NextChild(child, bitmap);
    } while (child);
}