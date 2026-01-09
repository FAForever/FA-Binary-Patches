#include "Allocator.h"
#include "LuaAllocator.h"

void *__cdecl MP_ReallocFunction(
    void *ptr,
    size_t oldsize,
    size_t size,
    void *data,
    const char *allocName,
    size_t flags)
{
    return static_cast<LuaAllocator *>(data)->Realloc(ptr, oldsize, size);
}

void __cdecl MP_FreeFunction(void *ptr, size_t oldsize, void *data)
{
    static_cast<LuaAllocator *>(data)->Free(ptr, oldsize);
}

SHARED LuaState *__thiscall UI_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("UI_StateCreate: %p", _this);
    LuaAllocator *pool = new (std::nothrow) LuaAllocator();

    if (pool)
        lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);

    new (_this) LuaState(libs);

    lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);

    return _this;
}

SHARED void __thiscall UI_StateDestroy(LuaState *_this)
{
    LogF("UI_StateDestroy: %p", _this);
    void *memData = lua_getMemData(_this->m_state);
    _this->~LuaState();
    if (memData)
    {
        delete (LuaAllocator *)memData;
    }
}

SHARED LuaState *__thiscall SIM_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("SIM_StateCreate: %p", _this);

    LuaAllocator *pool = new (std::nothrow) LuaAllocator();

    if (pool)
        lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);

    new (_this) LuaState(libs);

    lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);

    return _this;
}

SHARED void __thiscall SIM_StateDestroy(LuaState *_this)
{
    LogF("SIM_StateDestroy: %p", _this);
    void *memData = lua_getMemData(_this->m_state);
    _this->~LuaState();
    if (memData)
    {
        delete (LuaAllocator *)memData;
    }
}