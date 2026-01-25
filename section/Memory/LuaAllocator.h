#pragma once
#include "FixedPool.h"
#include "FixedSizePool.h"

constexpr size_t TABLE_HASH_SIZE = 40;
constexpr size_t TABLE_ARRAY_SIZE = 8;
constexpr size_t TABLE_SIZE = 36;
constexpr size_t UPVALUE_SIZE = 20;
constexpr size_t PARSER_LOCAL_SIZE = 12;
constexpr size_t SMALL_SIZE = 512;

#ifdef DEBUG
#define DBG_LOG(...) LogF(__VA_ARGS__);
#else
#define DBG_LOG(...)
#endif

class LuaAllocator
{
public:
    LuaAllocator()
        : stats{},
          logging_enabled{false},
          table_pool{},
          table_hash_pool{},
          table_array_pool{},
          upvalue_pool{},
          small_pool{},
          large_pool{}
    {
    }

    ~LuaAllocator() {}

    struct TypeFlags
    {
        bool is_table : 1;
        bool is_upvalue : 1;
        bool is_table_hash : 1;
        bool is_table_array : 1;
        bool is_small : 1;
        bool is_large : 1;

        operator bool() const
        {
            return is_table ||
                   is_upvalue ||
                   is_table_hash ||
                   is_table_array ||
                   is_small ||
                   is_large;
        }
    };

    struct Statistics
    {
        size_t allocations;
        size_t malloc_allocations;
        size_t reallocations;
        size_t realloc_fails;
        size_t realloc_misses;
        size_t free_misses;
        size_t allocator_to_malloc;
    };

    struct AllocationStatistics
    {
        Statistics stats;
        AllocationInfo table_info;
        AllocationInfo table_hash_info;
        AllocationInfo table_array_info;
        AllocationInfo upvalue_info;
        AllocationInfo small_info;
        AllocationInfo large_info;
    };

    TypeFlags GetTypeFlags(size_t size)
    {
        if (size == UPVALUE_SIZE)
            return {.is_upvalue = true};

        if (size == TABLE_SIZE)
            return {
                .is_table = true,
            };

        size_t n_hash = size / TABLE_HASH_SIZE;
        size_t n_array = size / TABLE_ARRAY_SIZE;

        return {
            .is_table_hash = size % TABLE_HASH_SIZE == 0 && IsPowerOf2(n_hash) && size <= 80 * 32,
            .is_table_array = size % TABLE_ARRAY_SIZE == 0 && IsPowerOf2(n_array) && size <= 32 * 32,
            .is_small = size <= SMALL_SIZE,
            .is_large = size > SMALL_SIZE && size <= 16384, // size that defines as small block in malloc
        };
    }

    void *Realloc(void *ptr, size_t old_size, size_t new_size)
    {
        if (ptr == nullptr || old_size == 0)
        {
            stats.allocations++;
            return Alloc(new_size);
        }

        if (new_size == 0)
        {
            Free(ptr, old_size);
            return nullptr;
        }
        if (logging_enabled)
        {
            LogF("Realloc: %d -> %d", old_size, new_size);
        }

        DBG_LOG("Realloc: %p %d %d", ptr, old_size, new_size);

        TypeFlags new_flags = GetTypeFlags(new_size);
        TypeFlags old_flags = GetTypeFlags(old_size);

        // case when both sizes are beyond limits of any pool
        if (!new_flags && !old_flags)
        {
            void *result = realloc(ptr, new_size);
            if (result != ptr)
            {
                stats.realloc_fails++;
            }
            return result;
        }

        // case when both sizes are within limits of any pool and we can realloc
        if (new_flags && old_flags)
        {
            void *result = nullptr;
            if (new_flags.is_table_hash)
                result = table_hash_pool.Realloc(ptr, old_size, new_size);
            else if (new_flags.is_table_array)
                result = table_array_pool.Realloc(ptr, old_size, new_size);
            else if (new_flags.is_small)
                result = small_pool.Realloc(ptr, old_size, new_size);
            else if (new_flags.is_large)
                result = large_pool.Realloc(ptr, old_size, new_size);

            if (result != nullptr)
            {
                stats.reallocations++;
                return result;
            }
            stats.realloc_misses++;
        }
        else
        {
            stats.allocator_to_malloc++;
        }

        void *new_ptr = InternalAlloc(new_size, new_flags);
        if (new_ptr == nullptr)
            return nullptr;

        memcpy(new_ptr, ptr, std::min(old_size, new_size));
        InternalFree(ptr, old_size, old_flags);
        return new_ptr;
    }

    void *Alloc(size_t size)
    {
        if (logging_enabled)
        {
            LogF("Alloc: %d", size);
        }
        TypeFlags flags = GetTypeFlags(size);
        void *ptr = InternalAlloc(size, flags);
        DBG_LOG("Alloc: %p %d", ptr, size);
        return ptr;
    }

    void Free(void *ptr, size_t size)
    {
        if (logging_enabled)
        {
            LogF("Free: %d", size);
        }
        DBG_LOG("Free: %p %d", ptr, size);
        TypeFlags flags = GetTypeFlags(size);
        InternalFree(ptr, size, flags);
    }

    AllocationStatistics GetStats() const
    {
        return {
            .stats = stats,
            .table_info = table_pool.GetInfo(),
            .table_hash_info = table_hash_pool.GetInfo(),
            .table_array_info = table_array_pool.GetInfo(),
            .upvalue_info = upvalue_pool.GetInfo(),
            .small_info = small_pool.GetInfo(),
            .large_info = large_pool.GetInfo(),
        };
    }

    static void *__cdecl ReallocF(
        void *ptr,
        size_t old_size,
        size_t new_size,
        void *data,
        const char *,
        size_t)
    {
        return static_cast<LuaAllocator *>(data)->Realloc(ptr, old_size, new_size);
    }

    static void __cdecl FreeF(void *ptr, size_t old_size, void *data)
    {
        static_cast<LuaAllocator *>(data)->Free(ptr, old_size);
    }

    void SetLogging(bool enabled)
    {
        logging_enabled = enabled;
    }

private:
    void InternalFree(void *ptr, size_t size, TypeFlags flags)
    {
        DBG_LOG("Free: %p %d", ptr, size);

        if (flags.is_table && table_pool.Free(ptr))
            return;
        if (flags.is_table_hash && table_hash_pool.Free(ptr, size))
            return;
        if (flags.is_table_array && table_array_pool.Free(ptr, size))
            return;
        if (flags.is_upvalue && upvalue_pool.Free(ptr))
            return;
        if (flags.is_small && small_pool.Free(ptr, size))
            return;
        if (flags.is_large && large_pool.Free(ptr, size))
            return;

        if (flags && FreeFromAll(ptr, size))
            return;

        free(ptr);
    }

    void *InternalAlloc(size_t size, TypeFlags flags)
    {
        if (flags.is_table_hash)
            return table_hash_pool.Alloc(size);
        else if (flags.is_table_array)
            return table_array_pool.Alloc(size);
        else if (flags.is_table)
            return table_pool.Alloc();
        else if (flags.is_upvalue)
            return upvalue_pool.Alloc();
        else if (flags.is_small)
            return small_pool.Alloc(size);
        else if (flags.is_large)
            return large_pool.Alloc(size);

        stats.malloc_allocations++;
        return malloc(size);
    }

    bool FreeFromAll(void *ptr, size_t size)
    {
        DBG_LOG("Free miss: %p %d", ptr, size);
        stats.free_misses++;

        return small_pool.Free(ptr, size) ||
               table_array_pool.Free(ptr, size) ||
               table_hash_pool.Free(ptr, size) ||
               table_pool.Free(ptr) ||
               upvalue_pool.Free(ptr) ||
               large_pool.Free(ptr, size);
    }

private:
    Statistics stats;
    bool logging_enabled;
    FixedSizePool<36, 64 * 1024> table_pool;
    FixedPool<80, 64 * 1024> table_hash_pool;
    FixedPool<32, 64 * 1024> table_array_pool;
    FixedSizePool<20, 64 * 1024> upvalue_pool;
    FixedPool<32, 64 * 1024> small_pool;
    FixedPool<512, 256> large_pool;
};
