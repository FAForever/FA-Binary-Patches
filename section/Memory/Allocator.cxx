#include "Allocator.h"
#include "LuaAllocator.h"

#include "magic_classes.h"

bool use_custom_allocator = false;

ConDescReg use_sim_allocator_convar{"sim_allocator", "", &use_custom_allocator};

int64_t total_cycles = 0;
size_t total_alloc = 0;
void *__cdecl MPPerf_ReallocFunction(
    void *ptr,
    size_t oldsize,
    size_t size,
    void *data,
    const char *allocName,
    size_t flags)
{
    int64_t start = ClockCycles();
    void *result = static_cast<LuaAllocator *>(data)->Realloc(ptr, oldsize, size);
    int64_t end = ClockCycles();
    total_alloc++;
    total_cycles += end - start;
    return result;
}

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

void *__cdecl Def_ReallocFunction(
    void *ptr,
    size_t oldsize,
    size_t size,
    void *data,
    const char *allocName,
    size_t flags)
{
    int64_t start = ClockCycles();
    void *result = realloc(ptr, size);
    int64_t end = ClockCycles();
    total_alloc++;
    total_cycles += end - start;
    return result;
}

void __cdecl Def_FreeFunction(void *ptr, size_t oldsize, void *data)
{
    free(ptr);
}

SHARED LuaState *__thiscall UI_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("UI_StateCreate: %p", _this);
    int64_t start = ClockCycles();

    LuaAllocator *pool = nullptr;
    // pool = new (std::nothrow) LuaAllocator();

    // if (pool)
    //     lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);
    // lua_setdefaultmemoryfunctions(Def_ReallocFunction, Def_FreeFunction, pool);

    new (_this) LuaState(libs);

    lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);
    int64_t end = ClockCycles();
    LogF("UI_StateCreate: %llu", CyclesToMicroSeconds(end - start));

    return _this;
}

SHARED void __thiscall UI_StateDestroy(LuaState *_this)
{
    LogF("UI_StateDestroy: %p", _this);
    LuaAllocator *memData = static_cast<LuaAllocator *>(lua_getMemData(_this->m_state));

    _this->~LuaState();

    delete memData;

    // LogF("Total Allocations: %u, Total Cycles: %llu", total_alloc, CyclesToMicroSeconds(total_cycles));
    // total_alloc = 0;
    // total_cycles = 0;
}
// UI
//  Fixed pool Total Allocations: 2192951, Total Cycles: 274446
//  default    Total Allocations: 2315488, Total Cycles: 138446

// SIM
//  Fixed pool Total Allocations: 8953037, Total Cycles: 5922986
//             Total Allocations: 8864595, Total Cycles: 5018217
//             Total Allocations: 9041332, Total Cycles: 5513742
//             Total Allocations: 8754166, Total Cycles: 5834894
//             Total Allocations: 8967810, Total Cycles:  890016
//  default    Total Allocations: 9075016, Total Cycles:  641968
//             Total Allocations: 8685327, Total Cycles:  590648

SHARED LuaState *__thiscall SIM_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("SIM_StateCreate: %p", _this);

    LuaAllocator *pool = nullptr;

    if (use_custom_allocator)
    {
        pool = new (std::nothrow) LuaAllocator();
    }

    if (pool)
        lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);

    new (_this) LuaState(libs);

    lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);

    return _this;
}

SHARED void __thiscall SIM_StateDestroy(LuaState *_this)
{
    LogF("SIM_StateDestroy: %p", _this);

    LuaAllocator *memData = static_cast<LuaAllocator *>(lua_getMemData(_this->m_state));

    _this->~LuaState();

    delete memData;

    LogF("Total Allocations: %u, Total Cycles: %llu", total_alloc, CyclesToMicroSeconds(total_cycles));
    total_alloc = 0;
    total_cycles = 0;
}