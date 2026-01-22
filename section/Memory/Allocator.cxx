#include "Allocator.h"
#include "LuaAllocator.h"
#include "magic_classes.h"
#include "LuaAPI.h"

bool sim_custom_allocator = true;

ConDescReg use_sim_allocator_convar{"sim_allocator", "", &sim_custom_allocator};

bool ui_custom_allocator = true;

ConDescReg use_ui_allocator_convar{"ui_allocator", "", &ui_custom_allocator};

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
// SimLua reprsl(GC_stats())
int GCStats(lua_State *L)
{
    LuaAllocator *memData = static_cast<LuaAllocator *>(lua_getMemData(L));
    if (memData == nullptr)
        return 0;

    const LuaAllocator::Statistics &stats = memData->GetStats();

    LuaObject obj{L->LuaState};
    obj.AssignNewTable(L->LuaState, 0, 0);

    obj.SetInteger("allocations", stats.allocations);
    obj.SetInteger("malloc_allocations", stats.malloc_allocations);
    obj.SetInteger("reallocations", stats.reallocations);
    obj.SetInteger("realloc_fails", stats.realloc_fails);
    obj.SetInteger("realloc_misses", stats.realloc_misses);
    obj.SetInteger("free_misses", stats.free_misses);
    obj.SetInteger("allocator_to_malloc", stats.allocator_to_malloc);

    obj.PushStack(L);

    return 1;
}

SimRegFunc sim_gc_stats_reg{"GC_stats", "", GCStats};
UIRegFunc ui_gc_stats_reg{"GC_stats", "", GCStats};

SHARED LuaState *__thiscall UI_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("UI_StateCreate: %p", _this);

    LuaAllocator *pool = nullptr;
    if (ui_custom_allocator)
        pool = new (std::nothrow) LuaAllocator();

    if (pool)
        lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);
    else
        lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);
    // lua_setdefaultmemoryfunctions(Def_ReallocFunction, Def_FreeFunction, pool);

    new (_this) LuaState(libs);

    lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);

    return _this;
}

SHARED void __thiscall UI_StateDestroy(LuaState *_this)
{
    LogF("UI_StateDestroy: %p", _this);
    LuaAllocator *memData = static_cast<LuaAllocator *>(lua_getMemData(_this->m_state));

    _this->~LuaState();

    delete memData;
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
// malloc/free    Total Allocations: 11474933, Total Cycles: 846484
// Lua allocator  Total Allocations: 11474955, Total Cycles: 942172

// malloc/free   Total Allocations: 11474954, Total Cycles: 842949
// Lua allocator Total Allocations: 11474925, Total Cycles: 962590

// malloc/free   Total Allocations: 9373585, Total Cycles: 687230
// Lua allocator Total Allocations: 9373612, Total Cycles: 719555
//               Total Allocations: 9373605, Total Cycles: 707989

SHARED LuaState *__thiscall SIM_StateCreate(LuaState *_this, StandardLibraries libs)
{
    LogF("SIM_StateCreate: %p", _this);

    LuaAllocator *pool = nullptr;

    if (sim_custom_allocator)
    {
        pool = new (std::nothrow) LuaAllocator();
    }

    if (pool)
        lua_setdefaultmemoryfunctions(MP_ReallocFunction, MP_FreeFunction, pool);
    else
        lua_setdefaultmemoryfunctions(nullptr, nullptr, nullptr);

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