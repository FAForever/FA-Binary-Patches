#include "include/moho.h"

#define g_UINextDesc	GPtr(0x0F59690, luaFuncDescReg*)
#define g_SimNextDesc	GPtr(0x0F5A124, luaFuncDescReg*)

extern void *UIFuncRegs[], *SimFuncRegs[];

void LuaFuncsReg() {
    luaFuncDescReg **descs;
    for (descs = UIFuncRegs; *descs; descs++) {
        (*descs)->Next = g_UINextDesc;
        g_UINextDesc = *descs;
    }
    for (descs = SimFuncRegs; *descs; descs++) {
        (*descs)->Next = g_SimNextDesc;
        g_SimNextDesc = *descs;
    }
}