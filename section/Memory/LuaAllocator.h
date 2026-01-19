#pragma once
#include "FixedPool.h"

constexpr size_t TABLE_HASH_SIZE = 40;
constexpr size_t TABLE_ARRAY_SIZE = 8;
constexpr size_t TABLE_SIZE = 36;
constexpr size_t UPVALUE_SIZE = 20;
constexpr size_t PARSER_LOCAL_SIZE = 12;
constexpr size_t SMALL_SIZE = 256;

#ifdef DEBUG
#define DBG_LOG(...) LogF(__VA_ARGS__);
#else
#define DBG_LOG(...)
#endif

class LuaAllocator
{
public:
    LuaAllocator()
        : table_pool{},
          table_hash_pool{},
          table_array_pool{},
          upvalue_pool{},
          parser_pool{},
          small_pool{}
    {
    }

    ~LuaAllocator()
    {
    }

    struct TypeFlags
    {
        bool is_parser;
        bool is_table;
        bool is_upvalue;
        bool is_table_hash;
        bool is_table_array;
        bool is_small;

        operator bool() const
        {
            return is_parser || is_table || is_upvalue || is_table_hash || is_table_array || is_small;
        }
    };

    TypeFlags GetTypeFlags(size_t size)
    {
        bool is_parser = size % PARSER_LOCAL_SIZE == 0 && size <= 12 * 32;
        //*
        return {
            .is_parser = is_parser,
            .is_table = size == TABLE_SIZE,
            .is_upvalue = size == UPVALUE_SIZE,
            .is_table_hash = size % TABLE_HASH_SIZE == 0 && size <= 80 * 32,
            .is_table_array = size % TABLE_ARRAY_SIZE == 0 && size <= 32 * 32 && !is_parser,
            .is_small = size <= SMALL_SIZE,
        };
        //*/

        /*
        return {
            .is_table = size == TABLE_SIZE,
            .is_upvalue = size == UPVALUE_SIZE,
            .is_table_hash = size % TABLE_HASH_SIZE == 0 && size <= 80 * 32,
            .is_small = size <= SMALL_SIZE,
            // .is_table_array = size % TABLE_ARRAY_SIZE == 0 && size <= 32 * 32,
        };
        //*/
    }

    void *Realloc(void *ptr, size_t old_size, size_t new_size)
    {
        if (ptr == nullptr || old_size == 0)
        {
            return Alloc(new_size);
        }

        if (new_size == 0)
        {
            Free(ptr, old_size);
            return nullptr;
        }

        DBG_LOG("Realloc: %p %d %d", ptr, old_size, new_size);

        TypeFlags flags = GetTypeFlags(new_size);

        void *result = nullptr;
        if (flags.is_table_hash)
            result = table_hash_pool.Realloc(ptr, old_size, new_size);
        else if (flags.is_table_array)
            result = table_array_pool.Realloc(ptr, old_size, new_size);
        else if (flags.is_parser)
            result = parser_pool.Realloc(ptr, old_size, new_size);
        else if (flags.is_small)
            result = small_pool.Realloc(ptr, old_size, new_size);

        if (result != nullptr)
            return result;

        void *new_ptr = InternalAlloc(new_size, flags);
        if (new_ptr)
        {
            memcpy(new_ptr, ptr, std::min(old_size, new_size));
            Free(ptr, old_size);
            return new_ptr;
        }
        return nullptr;
    }

    void *Alloc(size_t size)
    {
        TypeFlags flags = GetTypeFlags(size);
        void *ptr = InternalAlloc(size, flags);
        DBG_LOG("Alloc: %p %d", ptr, size);
        return ptr;
    }

    void Free(void *ptr, size_t size)
    {
        DBG_LOG("Free: %p %d", ptr, size);
        TypeFlags flags = GetTypeFlags(size);

        if (flags.is_table && table_pool.Free(ptr, size))
            return;
        if (flags.is_table_hash && table_hash_pool.Free(ptr, size))
            return;
        if (flags.is_table_array && table_array_pool.Free(ptr, size))
            return;
        if (flags.is_upvalue && upvalue_pool.Free(ptr, size))
            return;
        if (flags.is_parser && parser_pool.Free(ptr, size))
            return;
        if (flags.is_small && small_pool.Free(ptr, size))
            return;

        if (!flags || !FreeFromAll(ptr, size))
            free(ptr);
    }

private:
    void *InternalAlloc(size_t size, const TypeFlags &flags)
    {
        if (flags.is_table_hash)
            return table_hash_pool.Alloc(size);
        else if (flags.is_table_array)
            return table_array_pool.Alloc(size);
        else if (flags.is_table)
            return table_pool.Alloc(size);
        else if (flags.is_upvalue)
            return upvalue_pool.Alloc(size);
        else if (flags.is_parser)
            return parser_pool.Alloc(size);
        else if (flags.is_small)
            return small_pool.Alloc(size);
        else
            return malloc(size);
    }

    bool FreeFromAll(void *ptr, size_t size)
    {
        WarningF("Free miss: %p %d", ptr, size);
        return small_pool.Free(ptr, size) ||
               table_array_pool.Free(ptr, size) ||
               table_hash_pool.Free(ptr, size) ||
               table_pool.Free(ptr, size) ||
               upvalue_pool.Free(ptr, size) ||
               parser_pool.Free(ptr, size);
    }

private:
    FixedPool<36, 64 * 1024> table_pool;
    FixedPool<80, 64 * 1024> table_hash_pool;
    FixedPool<32, 64 * 1024> table_array_pool;
    FixedPool<20, 64 * 1024> upvalue_pool;
    FixedPool<12, 64 * 1024> parser_pool;
    FixedPool<32, 64 * 1024> small_pool;
};
