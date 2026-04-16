#pragma once
#include <cstdint>
#include <new>
#include <concepts>
#include <limits>
#include <bit>
#include <array>
#include <compare>
#include "Memory.h"
#include "LinkedList.h"

constexpr bool IsPowerOf2(size_t value)
{
    return std::has_single_bit(value);
}

template <auto V>
concept is_power_of_two = IsPowerOf2(V);

constexpr size_t CeilLog2(size_t x)
{
    return x <= 1 ? 0 : std::bit_width(x - 1);
}

constexpr size_t Mask(size_t bits)
{
    if (bits >= std::numeric_limits<size_t>::digits)
    {
        return std::numeric_limits<size_t>::max();
    }

    return ((size_t)1 << bits) - 1;
}

constexpr size_t GetFirstSetBit(size_t value)
{
    assert(value != 0);
    return std::countr_zero(value);
}

constexpr size_t NUM_BITS = std::numeric_limits<size_t>::digits;
constexpr size_t BITS_MASK = NUM_BITS - 1;
constexpr size_t INDEX_MASK = ~BITS_MASK;
constexpr size_t OFFSET = CeilLog2(NUM_BITS);
constexpr size_t FREE_SECTIONS_SIZE = 64;
constexpr size_t MAX_INDEX_DIST = 64;

using Byte = std::byte;

class BitIndex
{
public:
    BitIndex() : data{0} {}
    BitIndex(size_t bit_index) : data{bit_index} {}
    BitIndex(size_t index, size_t bit) : BitIndex((index << OFFSET) | bit & BITS_MASK) {}

    size_t Raw() const
    {
        return data;
    }

    BitIndex &operator++()
    {
        ++data;
        return *this;
    }

    BitIndex &operator--()
    {
        --data;
        return *this;
    }

    BitIndex operator++(int)
    {
        return {data++};
    }

    BitIndex operator--(int)
    {
        return {data--};
    }

    size_t Bit() const
    {
        return data & BITS_MASK;
    }

    size_t Index() const
    {
        return (data & INDEX_MASK) >> OFFSET;
    }

    BitIndex operator+(std::integral auto value) const
    {
        return {data + value};
    }

    BitIndex &operator+=(std::integral auto value)
    {
        data += value;
        return *this;
    }

private:
    size_t data;
};

template <size_t BITS_COUNT>
class BitArray
{
    static_assert(BITS_COUNT % NUM_BITS == 0, "BITS_COUNT");
    static const size_t SIZE = BITS_COUNT / NUM_BITS;

public:
    BitArray() = default;

    ~BitArray() = default;

    bool Get(BitIndex bit_index) const
    {
        return (bits[bit_index.Index()] >> bit_index.Bit()) & 0b1;
    }

    void Set(BitIndex bit_index, bool value)
    {
        size_t bit = bit_index.Bit();
        size_t *p_value = &GetSection(bit_index.Index());
        size_t cur_value = *p_value;
        size_t new_value = cur_value & ~((size_t)1 << bit) | (static_cast<size_t>(value) << bit);
        *p_value = new_value;
    }

    size_t &GetSection(size_t index)
    {
        return bits[index];
    }

    void Set(BitIndex bit_index)
    {
        size_t bit = bit_index.Bit();
        size_t index = bit_index.Index();
        GetSection(index) |= ((size_t)1 << bit);
    }

    void Reset(BitIndex bit_index)
    {
        size_t bit = bit_index.Bit();
        size_t index = bit_index.Index();
        GetSection(index) &= ~((size_t)1 << bit);
    }

    size_t GetSize() const { return SIZE; }

    size_t GetSizeBits() const { return BITS_COUNT; }

private:
    size_t bits[SIZE]{};
};

template <size_t SIZE>
class CellSize
{
    static size_t CountCells(size_t bytes)
    {
        return bytes / SIZE + (bytes % SIZE == 0 ? 0 : 1);
    }

public:
    CellSize() : CellSize(0)
    {
    }

    explicit CellSize(size_t bytes) : size{CountCells(bytes)}
    {
    }

    CellSize(const CellSize &rhs) = default;
    CellSize &operator=(const CellSize &rhs) = default;

    CellSize(CellSize &&rhs) = default;
    CellSize &operator=(CellSize &&rhs) = default;

    ~CellSize() = default;

    std::strong_ordering operator<=>(const CellSize &rhs) const { return size <=> rhs.size; }

    operator size_t() const { return Cells(); }

    size_t Bytes() const { return size * SIZE; }
    size_t Cells() const { return size; }

private:
    size_t size;
};

template <size_t CELL_SIZE, size_t CELLS_IN_CHUNK>
class Chunk
{
private:
    static_assert(CELL_SIZE % sizeof(size_t) == 0, "CELL_SIZE must be divisible by size pointer");
    static_assert(CELLS_IN_CHUNK % NUM_BITS == 0, "CELLS_PER_CHUNK");

    static const size_t CHUNK_SIZE = CELL_SIZE * CELLS_IN_CHUNK;

    using SelfT = Chunk<CELL_SIZE, CELLS_IN_CHUNK>;

    void *At(BitIndex bit_index)
    {
        return Begin() + bit_index.Raw() * CELL_SIZE;
    }

    size_t GetIndexByPtr(void *ptr)
    {
        return (static_cast<Byte *>(ptr) - Begin()) / CELL_SIZE;
    }

    size_t CountFree(BitIndex start, size_t max_count) const
    {
        if ((start + max_count).Raw() >= CELLS_IN_CHUNK)
            return 0;

        for (size_t offset = 0; offset < max_count; ++offset)
        {
            if (!bits.Get(start + offset))
            {
                return offset;
            }
        }
        return max_count;
    }

    void *Use1Cell(BitIndex bit_index)
    {
        used_cells++;
        bits.Reset(bit_index);
        return At(bit_index);
    }

    void *UseNCellsSmall(size_t index, size_t offset, size_t cells, size_t mask)
    {
        used_cells += cells;
        bits.GetSection(index) &= ~(mask << offset);
        return At(BitIndex{index, offset});
    }

    void *UseNCells(BitIndex bit_index, size_t cells)
    {
        used_cells += cells;
        for (size_t offset = 0; offset < cells; ++offset)
        {
            bits.Reset(bit_index + offset);
        }
        return At(bit_index);
    }

    bool AddToFreeList(size_t index, size_t pow)
    {
        failed_powers[pow] = false;

        size_t min_diff = std::numeric_limits<size_t>::max();
        ptrdiff_t min_diff_index = 0;
        size_t (&bucket)[FREE_SECTIONS_SIZE] = free_sections[pow];
        for (size_t i = 0; i < std::size(bucket); i++)
        {
            size_t sect_index = bucket[i];

            size_t diff = std::abs((ptrdiff_t)sect_index - (ptrdiff_t)index);
            if (diff < min_diff)
            {
                min_diff = diff;
                min_diff_index = i;
            }
        }
        // replace with nearest possible
        if (min_diff <= MAX_INDEX_DIST)
        {
            size_t sect_index = bucket[min_diff_index];
            if (index < sect_index)
            {
                bucket[min_diff_index] = index;
            }
            return true;
        }

        // add to free slot
        for (size_t i = 0; i < std::size(bucket); i++)
        {
            size_t sect_index = bucket[i];
            if (sect_index == 0)
            {
                bucket[i] = index;
                return true;
            }
        }
        // no free slot
        return false;
    }

    void *Alloc1Cell()
    {
        if (!ReachedEnd())
        {
            for (size_t i = top_index; i < bits.GetSize(); i++)
            {
                size_t section = bits.GetSection(i);
                if (section == 0)
                    continue;

                size_t bit = GetFirstSetBit(section);
                top_index = i;
                return Use1Cell(BitIndex{i, bit});
            }
            top_index = bits.GetSize();
        }

        // size_t (&bucket)[FREE_SECTIONS_SIZE] = free_sections[0];

        // // then we check free list
        // for (size_t i = 0; i < std::size(bucket); i++)
        // {
        //     size_t index = bucket[i];
        //     size_t section = bits.GetSection(index);

        //     if (section == 0) // invalidate
        //     {
        //         bucket[i] = 0;
        //         continue;
        //     }

        //     size_t bit = GetFirstSetBit(section);
        //     return Use1Cell(BitIndex{index, bit});
        // }

        for (size_t i = start_index; i < bits.GetSize(); i++)
        {
            size_t section = bits.GetSection(i);
            if (section == 0)
                continue;

            start_index = i;
            size_t bit = GetFirstSetBit(section);
            return Use1Cell(BitIndex{i, bit});
        }

        start_index = 0;
        return nullptr;
    }

    void *AllocNCellsSmall(size_t cells)
    {
        size_t step = IsPowerOf2(cells) ? cells : std::bit_ceil(cells);
        size_t mask = Mask(cells);
        size_t pow = CeilLog2(cells);

        if (failed_powers[pow])
            return nullptr;

        // First go forward
        if (!ReachedEnd())
        {
            for (size_t i = top_index; i < bits.GetSize(); i++)
            {
                size_t section = bits.GetSection(i);
                if (section == 0)
                    continue;

                for (size_t offset = 0; offset < NUM_BITS; offset += step)
                {
                    if (((section >> offset) & mask) == mask)
                    {
                        top_index = i;
                        return UseNCellsSmall(i, offset, cells, mask);
                    }
                }
            }
            top_index = bits.GetSize();
        }

        size_t (&bucket)[FREE_SECTIONS_SIZE] = free_sections[pow];

        // then we check free list
        for (size_t i = 0; i < std::size(bucket); i++)
        {
            size_t index = bucket[i];
            size_t section = bits.GetSection(index);

            if (section == 0) // invalidate
            {
                bucket[i] = 0;
                continue;
            }

            for (size_t offset = 0; offset < NUM_BITS; offset += step)
            {
                if (((section >> offset) & mask) == mask)
                {
                    return UseNCellsSmall(index, offset, cells, mask);
                }
            }

            // invalidate too
            bucket[i] = 0;
        }

        for (size_t i = start_index; i < bits.GetSize(); i++)
        {
            size_t section = bits.GetSection(i);
            if (section == 0)
                continue;

            for (size_t offset = 0; offset < NUM_BITS; offset += step)
            {
                if (((section >> offset) & mask) == mask)
                {
                    return UseNCellsSmall(i, offset, cells, mask);
                }
            }
        }
        start_index = 0;
        failed_powers[pow] = true;
        return nullptr;
    }

    void *AllocNCellsLarge(size_t cells)
    {
        //! TODO
        return nullptr;
    }

    void *AllocNCells(size_t cells)
    {
        if (cells == 0 || cells > CELLS_IN_CHUNK)
            return nullptr;

        if (cells <= NUM_BITS) [[likely]]
        {
            return AllocNCellsSmall(cells);
        }
        return AllocNCellsLarge(cells);
    }

    void FreeCells(void *ptr, size_t cells)
    {
        used_cells -= cells;
        BitIndex bit_index{GetIndexByPtr(ptr)};

        if (cells == 1)
        {
            bits.Set(bit_index);
            return;
        }

        size_t index = bit_index.Index();
        size_t bit = bit_index.Bit();

        // cells fit into one section
        if (bit + cells <= NUM_BITS)
        {
            size_t mask = Mask(cells);
            bits.GetSection(index) |= (mask << bit);
        }
        else
        {
            for (size_t i = 0; i < cells; i++)
            {
                bits.Set(bit_index + i);
            }
        }

        AddToFreeList(index, CeilLog2(cells));
    }

public:
    Chunk()
        : top_index{0},
          start_index{0},
          used_cells{0},
          failed_powers{},
          free_sections{},
          bits{}
    {
        for (size_t i = 0; i < bits.GetSize(); i++)
        {
            bits.GetSection(i) = std::numeric_limits<size_t>::max();
        }
    }

    Chunk(const Chunk &) = delete;
    Chunk(Chunk &&) = delete;
    Chunk &operator=(const Chunk &) = delete;
    Chunk &operator=(Chunk &&) = delete;

    ~Chunk()
    {
        top_index = 0;
        start_index = 0;
        used_cells = 0;
    }

    struct ExtendResult
    {
        void *ptr;
        bool belongs;
    };

    ExtendResult Extend(void *ptr, size_t old_cells, size_t new_cells)
    {
        if (!BelongsToChunk(ptr))
            return {nullptr, false};

        if (old_cells == new_cells)
        {
            return {ptr, true};
        }

        BitIndex pos = GetIndexByPtr(ptr);
        if (old_cells < new_cells) // more space needed
        {
            size_t diff = new_cells - old_cells;
            size_t free_cells = CountFree(pos + old_cells, diff);
            if (free_cells == diff)
            {
                used_cells += diff;
                for (size_t offset = old_cells; offset < new_cells; ++offset)
                {
                    bits.Reset(pos + offset);
                }
                return {ptr, true};
            }
        }
        else // less space needed
        {
            used_cells -= old_cells - new_cells;
            for (size_t i = new_cells; i < old_cells; i++)
            {
                bits.Set(pos + i);
            }
            return {ptr, true};
        }

        return {nullptr, true};
    }

    void *Alloc(size_t cells)
    {
        if (used_cells + cells > CELLS_IN_CHUNK)
            return nullptr;

        return cells == 1
                   ? Alloc1Cell()
                   : AllocNCells(cells);
    }

    bool Free(void *ptr, size_t cells)
    {
        if (ptr == nullptr)
            return true;

        if (!BelongsToChunk(ptr))
            return false;

        FreeCells(ptr, cells);
        return true;
    }

    bool ReachedEnd() const
    {
        return top_index >= bits.GetSize();
    }

    Byte *Begin() { return data; }
    Byte *End() { return data + CHUNK_SIZE; }
    const Byte *Begin() const { return data; }
    const Byte *End() const { return data + CHUNK_SIZE; }

    bool BelongsToChunk(void *ptr) const
    {
        Byte *ptrb = static_cast<Byte *>(ptr);
        return Begin() <= ptrb && ptrb < End();
    }

    size_t GetUsedCells() const { return used_cells; }

    AllocationInfo GetInfo() const
    {
        return {
            .total_storage = CHUNK_SIZE,
            .occupied_storage = used_cells * CELL_SIZE,
        };
    }

private:
    size_t top_index;
    size_t start_index;
    size_t used_cells;
    bool failed_powers[OFFSET + 1];
    size_t free_sections[OFFSET + 1][FREE_SECTIONS_SIZE];
    BitArray<CELLS_IN_CHUNK> bits;
    Byte data[CHUNK_SIZE];
};

template <size_t CELL_SIZE, size_t CELLS_IN_CHUNK>
class FixedPool
{
    using ChunkT = Chunk<CELL_SIZE, CELLS_IN_CHUNK>;

    static_assert(CELL_SIZE % sizeof(size_t) == 0, "CELL_SIZE must be divisible by size pointer");
    static_assert(CELLS_IN_CHUNK % NUM_BITS == 0, "CELLS_IN_CHUNK");

    void *InternalAlloc(size_t cells)
    {
        ChunkT *last = last_successful;
        if (last != nullptr)
        {
            void *ptr = last->Alloc(cells);
            if (ptr != nullptr)
            {
                return ptr;
            }
        }
        last_successful = nullptr;

        for (ChunkT &chunk : chunks)
        {
            if (&chunk != last)
            {
                void *ptr = chunk.Alloc(cells);
                if (ptr != nullptr)
                {
                    last_successful = &chunk;
                    return ptr;
                }
            }
        }

        ChunkT *chunk = chunks.AddBack();
        if (chunk == nullptr) // can't create more!
            return nullptr;

        last_successful = chunk;
        return chunk->Alloc(cells);
    }

    bool InternalFree(void *ptr, size_t cells)
    {
        for (ChunkT &chunk : chunks)
        {
            if (chunk.Free(ptr, cells))
            {
                if (chunk.GetUsedCells() == 0)
                {
                    chunks.Remove(&chunk);
                    if (last_successful == &chunk)
                        last_successful = nullptr;
                }
                return true;
            }
        }
        return false;
    }

    void *InternalRealloc(void *ptr, size_t old_cells, size_t new_cells)
    {
        bool belongs = false;

        for (ChunkT &chunk : chunks)
        {
            auto r = chunk.Extend(ptr, old_cells, new_cells);
            if (r.ptr)
            {
                return r.ptr;
            }
            belongs = belongs || r.belongs;
        }
        // couldn't extend

        if (!belongs)
            return nullptr;

        void *new_ptr = InternalAlloc(new_cells);
        if (new_ptr == nullptr)
            return nullptr;

        // it is faster to copy aligned memory
        memcpy(new_ptr, ptr, CELL_SIZE * std::min(old_cells, new_cells));
        InternalFree(ptr, old_cells);
        return new_ptr;
    }

public:
    static size_t CountCells(size_t bytes)
    {
        return bytes / CELL_SIZE + (bytes % CELL_SIZE == 0 ? 0 : 1);
    }

    FixedPool()
        : last_successful{nullptr},
          chunks{}
    {
    }

    FixedPool(const FixedPool &) = delete;
    FixedPool(FixedPool &&) = delete;
    FixedPool &operator=(const FixedPool &) = delete;
    FixedPool &operator=(FixedPool &&) = delete;

    ~FixedPool()
    {
        last_successful = nullptr;
    }

    void *Realloc(void *ptr, size_t old_size, size_t new_size)
    {
        size_t old_cells = CountCells(old_size);
        size_t new_cells = CountCells(new_size);
        return InternalRealloc(ptr, old_cells, new_cells);
    }

    void *Alloc(size_t size)
    {
        size_t cells = CountCells(size);
        return InternalAlloc(cells);
    }

    bool Free(void *ptr, size_t size)
    {
        if (ptr == nullptr || size == 0)
            return true;

        return InternalFree(ptr, CountCells(size));
    }

    bool BelongsToPool(void *ptr) const
    {
        for (const ChunkT &chunk : chunks)
        {
            if (chunk.BelongsToChunk(ptr))
                return true;
        }
        return false;
    }

    size_t NumChunks() const
    {
        return chunks.Size();
    }

    AllocationInfo GetInfo() const
    {
        AllocationInfo info{};
        for (const ChunkT &chunk : chunks)
        {
            auto chunk_info = chunk.GetInfo();
            info.total_storage += chunk_info.total_storage;
            info.occupied_storage += chunk_info.occupied_storage;
            info.chunk_count++;
        }
        return info;
    }

private:
    ChunkT *last_successful;
    LinkedList<ChunkT> chunks;
};