#pragma once
#include <cstdint>
#include <new>
#include <concepts>
#include <limits>
#include <bit>
#include <array>
#include <compare>
#include "LinkedList.h"
#include "Memory.h"

template <size_t CELL_SIZE, size_t CELLS_IN_CHUNK>
class FixedSizePool
{
    class Chunk
    {
        static const size_t CHUNK_SIZE = CELL_SIZE * CELLS_IN_CHUNK;
        static_assert(CELL_SIZE % sizeof(size_t) == 0, "CELL_SIZE must be divisible by size pointer");

        struct Block
        {
            Block *next;
        };

    public:
        Chunk()
            : free_cells{CELLS_IN_CHUNK}
        {
            free_list = reinterpret_cast<Block *>(Begin());
            Block *current = free_list;
            for (size_t i = 0; i < CELLS_IN_CHUNK - 1; ++i)
            {
                current->next = reinterpret_cast<Block *>(reinterpret_cast<std::byte *>(current) + CELL_SIZE);
                current = current->next;
            }
            current->next = nullptr;
        }

        ~Chunk()
        {
        }

        void *Alloc()
        {
            if (!free_list)
                return nullptr;

            free_cells--;
            Block *allocated_block = free_list;
            free_list = free_list->next;
            return allocated_block;
        }

        std::byte *Begin() { return data; }
        std::byte *End() { return data + CHUNK_SIZE; }
        const std::byte *Begin() const { return data; }
        const std::byte *End() const { return data + CHUNK_SIZE; }

        bool BelongsToChunk(void *ptr) const
        {
            return reinterpret_cast<std::byte *>(ptr) >= Begin() &&
                   reinterpret_cast<std::byte *>(ptr) < End();
        }

        bool Free(void *ptr)
        {
            if (!ptr)
                return true;

            if (!BelongsToChunk(ptr))
                return false;

            free_cells++;
            Block *freed_block = static_cast<Block *>(ptr);
            freed_block->next = free_list;
            free_list = freed_block;
            return true;
        }

        AllocationInfo GetInfo() const
        {
            return
            {
                .total_storage = CHUNK_SIZE,
                .occupied_storage = CHUNK_SIZE - free_cells * CELL_SIZE
            };
        }

    private:
        Block *free_list;
        size_t free_cells;
        std::byte data[CHUNK_SIZE];
    };

public:
    FixedSizePool()
        : chunks{}
    {
    }

    ~FixedSizePool()
    {
    }

    void *Alloc()
    {
        for (Chunk &chunk : chunks)
        {
            void *ptr = chunk.Alloc();
            if (ptr)
                return ptr;
        }

        Chunk *c = chunks.AddBack();
        if (c == nullptr)
            return nullptr;

        return c->Alloc();
    }

    bool Free(void *ptr)
    {
        for (Chunk &chunk : chunks)
        {
            if (chunk.Free(ptr))
                return true;
        }
        return false;
    }

    AllocationInfo GetInfo() const
    {
        AllocationInfo info{};
        for (const Chunk &chunk : chunks)
        {
            auto chunk_info = chunk.GetInfo();
            info.total_storage += chunk_info.total_storage;
            info.occupied_storage += chunk_info.occupied_storage;
            info.chunk_count++;
        }
        return info;
    }

private:
    LinkedList<Chunk> chunks;
};