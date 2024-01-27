#include "include/moho.h"
#include "include/magic_classes.h"

#define lua_push(L, name, val) \
    lua_pushstring(L, name); \
    lua_pushnumber(L, val); \
    lua_rawset(L, -3);

UIRegFunc GetHighlightCommand{"GetHighlightCommand", "table? GetHighlightCommand() - return table of command or nil", +[](lua_State* L) {
    auto commandId = g_CWldSession->mouse.highlightCommandId;
    if (commandId == -1)
        return 0;
    volatile incomplete::Command* command;
    asm("push %[commandId] \n"
        "call 0x8B5BB0":"=a"(command) : "S"(g_CWldSession->unk3), [commandId]"g"(commandId));
    if (!command)
        return 0;

    lua_newtable(L);
    lua_push(L, "commandType", command->commandType);
    lua_push(L, "x", command->pos.x);
    lua_push(L, "y", command->pos.y);
    lua_push(L, "z", command->pos.z);
    auto targetId = command->targetId;
    if (targetId != 0xF0000000) {
        char buf[16];
        sprintf_s(buf, sizeof(buf), "%d", targetId);
        lua_pushstring(L, "targetId");
        lua_pushstring(L, buf);
        lua_rawset(L, -3);
    }
    auto blueprintId = command->blueprintId;
    if (blueprintId) {
        lua_pushstring(L, "blueprintId");
        lua_pushstring(L, blueprintId->data());
        lua_rawset(L, -3);
    }
    return 1;
}};
