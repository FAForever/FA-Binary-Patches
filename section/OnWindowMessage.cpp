#include "global.h"


struct KeyEventInfo
{
    unsigned long repeatCount : 16;
    unsigned long scanCode : 8;
    unsigned long isExtended : 1;
    unsigned long reserved : 4;
    unsigned long contextCode : 1;
    unsigned long previousState : 1;
    unsigned long transitionState : 1;

    inline unsigned long ToULong() const
    {
        return repeatCount |
               (scanCode << 16) |
               (isExtended << 24) |
               (reserved << 25) |
               (contextCode << 29) |
               (previousState << 30) |
               (transitionState << 31);
    }
};
static_assert(sizeof(KeyEventInfo) == 4);

int __thiscall wxWindow__MSWWindowProc(void *this_, unsigned int uMsg, unsigned int wParam, unsigned long lParam) asm("0x0096D110");

int __thiscall OnWindowMessage(void *this_, unsigned int uMsg, unsigned int wParam, unsigned long lParam)
{
    switch (uMsg)
    {
    case 0x020B:                                  /*WM_XBUTTONDOWN*/
        uMsg = 0x0100;                            /*WM_KEYDOWN*/
        wParam = (((wParam) >> 16) & 0xFFFF) + 4; // VK_XBUTTON(1 or 2)
        lParam = KeyEventInfo{
            .repeatCount = 1,
            .scanCode = 0xFF,
            .isExtended = 0,
            .reserved = 0,
            .contextCode = 0,
            .previousState = 0,
            .transitionState = 0}
                     .ToULong();
        break;
    case 0x020C:                                  /*WM_XBUTTONUP*/
        uMsg = 0x0101;                            /*WM_KEYUP*/
        wParam = (((wParam) >> 16) & 0xFFFF) + 4; // VK_XBUTTON(1 or 2)
        lParam = KeyEventInfo{
            .repeatCount = 1,
            .scanCode = 0xFF,
            .isExtended = 0,
            .reserved = 0,
            .contextCode = 0,
            .previousState = 1,
            .transitionState = 1}
                     .ToULong();
        break;
    default:
        break;
    }
    return wxWindow__MSWWindowProc(this_, uMsg, wParam, lParam);
}