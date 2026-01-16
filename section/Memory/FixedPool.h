#pragma once
#include <cstdint>
#include <new>
#include <concepts>
#include <limits>
#include <bit>

constexpr bool IsPowerOf2(size_t value)
{
    return (value > 0) && ((value & (value - 1)) == 0);
}

template <auto V>
concept is_power_of_two = IsPowerOf2(V);

constexpr size_t FloorLog2(size_t x)
{
    return x <= 1 ? 0 : 1 + FloorLog2(x >> 1);
}

constexpr size_t Align(size_t size, size_t align)
{
    return (size / align * align) + (size % align != 0 ? align : 0);
}

constexpr size_t NUM_BITS = std::numeric_limits<size_t>::digits;
constexpr size_t BITS_MASK = NUM_BITS - 1;
constexpr size_t INDEX_MASK = ~BITS_MASK;
constexpr size_t OFFSET = FloorLog2(NUM_BITS);

using Byte = unsigned char;

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
        size_t *p_value = &GetSection(bit_index.Index());
        size_t cur_value = *p_value;
        size_t new_value = cur_value | ((size_t)1 << bit);
        *p_value = new_value;
    }

    void Reset(BitIndex bit_index)
    {
        size_t bit = bit_index.Bit();
        size_t *p_value = &GetSection(bit_index.Index());
        size_t cur_value = *p_value;
        size_t new_value = cur_value & ~((size_t)1 << bit);
        *p_value = new_value;
    }

    size_t GetSize() const { return SIZE; }

    size_t GetSizeBits() const { return BITS_COUNT; }

private:
    size_t bits[SIZE]{};
};

template <size_t CELL_SIZE, size_t CELLS_IN_CHUNK>
class Chunk
{
    static_assert(CELL_SIZE % sizeof(size_t) == 0, "CELL_SIZE must be divisible by size pointer");
    static_assert(CELLS_IN_CHUNK % NUM_BITS == 0, "CELLS_PER_CHUNK");

    static const size_t CHUNK_SIZE = CELL_SIZE * CELLS_IN_CHUNK;

    using SelfT = Chunk<CELL_SIZE, CELLS_IN_CHUNK>;

    void *At(BitIndex bit_index)
    {
        return data + bit_index.Raw() * CELL_SIZE;
    }

    size_t GetIndexByPtr(void *ptr)
    {
        return (static_cast<Byte *>(ptr) - Begin()) / CELL_SIZE;
    }

    void *Use1Cell(BitIndex bit_index)
    {
        used_cells++;
        bits.Reset(bit_index);
        return At(bit_index);
    }

    void *UseNCells(BitIndex bit_index, size_t count)
    {
        used_cells += count;
        for (size_t offset = 0; offset < count; ++offset)
        {
            bits.Reset(bit_index + offset);
        }
        return At(bit_index);
    }

    static size_t GetFirstSetBit(size_t value)
    {
        assert(value != 0);
        size_t index = 0;
        while ((value & ((size_t)1 << index)) == 0)
        {
            index++;
        }
        return index;
    }

    static constexpr size_t Mask(size_t bits)
    {
        if (bits >= std::numeric_limits<size_t>::digits)
        {
            return std::numeric_limits<size_t>::max();
        }

        return ((size_t)1 << bits) - 1;
    }

    void *Alloc1Cell()
    {
        if (!ReachedEnd())
        {
            for (size_t i = top_index; i < bits.GetSize(); i++)
            {
                size_t sector = bits.GetSection(i);
                if (sector)
                {
                    size_t bit = GetFirstSetBit(sector);
                    top_index = i;
                    return Use1Cell(BitIndex{i, bit});
                }
            }
            top_index = bits.GetSize();
        }
        for (size_t i = 0; i < bits.GetSize(); i++)
        {
            size_t sector = bits.GetSection(i);
            if (sector)
            {
                size_t bit = GetFirstSetBit(sector);
                return Use1Cell(BitIndex{i, bit});
            }
        }

        return nullptr;
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

    void *AllocNCellsSmall(size_t cells)
    {
        size_t step = IsPowerOf2(cells) ? cells : std::bit_ceil(cells);
        size_t mask = Mask(cells);

        if (!ReachedEnd())
        {
            for (size_t i = top_index; i < bits.GetSize(); i++)
            {
                size_t section = bits.GetSection(i);
                if (section)
                {
                    for (size_t offset = 0; offset < NUM_BITS; offset += step)
                    {
                        if (((section >> offset) & mask) == mask)
                        {
                            top_index = i;
                            return UseNCells(BitIndex{i, offset}, cells);
                        }
                    }
                }
            }
            top_index = bits.GetSize();
        }

        for (size_t i = 0; i < bits.GetSize(); i++)
        {
            size_t section = bits.GetSection(i);
            if (section)
            {
                for (size_t offset = 0; offset < NUM_BITS; offset += step)
                {
                    if (((section >> offset) & mask) == mask)
                    {
                        return UseNCells(BitIndex{i, offset}, cells);
                    }
                }
            }
        }
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

        // ! logic for large chunks rn not supported

        return nullptr;
    }

public:
    static size_t CountCells(size_t bytes)
    {
        return bytes / CELL_SIZE + (bytes % CELL_SIZE == 0 ? 0 : 1);
    }

    Chunk()
        : top_index{0},
          used_cells{0},
          next{nullptr},
          bits{}
    {
        for (size_t i = 0; i < bits.GetSize(); i++)
        {
            bits.GetSection(i) = std::numeric_limits<size_t>::max();
        }
    }

    ~Chunk()
    {
        top_index = 0;
        used_cells = 0;
        delete next;
    }

    void *Extend(void *ptr, size_t old_size, size_t new_size)
    {
        if (!BelongsToChunk(ptr))
            return nullptr;

        size_t old_cells = CountCells(old_size);
        size_t new_cells = CountCells(new_size);
        if (old_cells == new_cells)
        {
            return ptr;
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
                return ptr;
            }
        }
        else // less space needed
        {
            used_cells -= old_cells - new_cells;
            for (size_t i = new_cells; i < old_cells; i++)
            {
                bits.Set(pos + i);
            }
            return ptr;
        }

        return nullptr;
    }

    template <size_t Cells>
        requires is_power_of_two<Cells>
    void *AllocCells()
    {
        static_assert(Cells <= NUM_BITS, "Cells <= NUM_BITS");
        constexpr size_t mask = Mask(Cells);
        if (!ReachedEnd())
        {
            for (size_t i = top_index; i < bits.GetSize(); i++)
            {
                size_t section = bits.GetSection(i);
                if (section)
                {
                    for (size_t offset = 0; offset < NUM_BITS; offset += Cells)
                    {
                        if (((section >> offset) & mask) == mask)
                        {
                            top_index = i;
                            return UseNCells(BitIndex{i, offset}, Cells);
                        }
                    }
                }
            }
            top_index = bits.GetSize();
        }

        for (size_t i = 0; i < bits.GetSize(); i++)
        {
            size_t section = bits.GetSection(i);
            if (section)
            {
                for (size_t offset = 0; offset < NUM_BITS; offset += Cells)
                {
                    if (((section >> offset) & mask) == mask)
                    {
                        return UseNCells(BitIndex{i, offset}, Cells);
                    }
                }
            }
        }
        return nullptr;
    }

    template <>
    void *AllocCells<1>()
    {
        return Alloc1Cell();
    }

    void *Alloc(size_t size)
    {
        size_t cells_count = CountCells(size);
        return cells_count == 1
                   ? Alloc1Cell()
                   : AllocNCells(cells_count);
    }

    bool Free(void *ptr, size_t old_size)
    {
        if (ptr == nullptr)
            return true;

        if (!BelongsToChunk(ptr))
            return false;

        size_t old_cells = CountCells(old_size);
        BitIndex bit_index{GetIndexByPtr(ptr)};

        used_cells -= old_cells;
        for (size_t i = 0; i < old_cells; i++)
        {
            bits.Set(bit_index + i);
        }
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

    SelfT *NextChunk() { return next; }
    const SelfT *NextChunk() const { return next; }

    SelfT *CreateNextChunk()
    {
        next = new (std::nothrow) SelfT();
        return next;
    }

private:
    size_t top_index;
    size_t used_cells;
    SelfT *next;
    BitArray<CELLS_IN_CHUNK> bits;
    Byte data[CHUNK_SIZE];
};

template <size_t CELL_SIZE, size_t CELLS_IN_CHUNK>
class FixedPool
{
    static_assert(CELL_SIZE % sizeof(size_t) == 0, "CELL_SIZE must be divisible by size pointer");
    static_assert(CELLS_IN_CHUNK % NUM_BITS == 0, "CELLS_IN_CHUNK");

public:
    using ChunkT = Chunk<CELL_SIZE, CELLS_IN_CHUNK>;

public:
    FixedPool()
        : num_chunks{1},
          head{new ChunkT()}
    {
    }

    ~FixedPool()
    {
        num_chunks = 0;
        delete head;
    }

    void *Realloc(void *ptr, size_t old_size, size_t new_size)
    {
        if (ptr == nullptr)
        {
            return Alloc(new_size);
        }

        if (new_size == 0)
        {
            Free(ptr, old_size);
            return nullptr;
        }

        if (!BelongsToPool(ptr))
            return nullptr;

        ChunkT *cur = head;
        ChunkT *prev = nullptr;
        void *new_ptr = nullptr;
        while (cur != nullptr)
        {
            new_ptr = cur->Extend(ptr, old_size, new_size);
            if (new_ptr != nullptr)
            {
                return new_ptr;
            }
            prev = cur;
            cur = cur->NextChunk();
        }
        // couldn't extend

        new_ptr = Alloc(new_size);
        if (new_ptr)
        {
            memcpy(new_ptr, ptr, std::min(old_size, new_size));
            Free(ptr, old_size);
            return new_ptr;
        }
        return nullptr;
    }

    template <size_t Cells>
        requires is_power_of_two<Cells>
    void *AllocCells()
    {
        ChunkT *cur = head;
        ChunkT *prev = nullptr;
        while (cur != nullptr)
        {
            void *ptr = cur->template AllocCells<Cells>();
            if (ptr != nullptr)
            {
                return ptr;
            }
            prev = cur;
            cur = cur->NextChunk();
        }

        cur = prev->CreateNextChunk();
        if (cur == nullptr) // can't create more!
            return nullptr;

        num_chunks++;
        return cur->template AllocCells<Cells>();
    }

    void *Alloc(size_t size)
    {
        ChunkT *cur = head;
        ChunkT *prev = nullptr;
        while (cur != nullptr)
        {
            void *ptr = cur->Alloc(size);
            if (ptr != nullptr)
            {
                return ptr;
            }
            prev = cur;
            cur = cur->NextChunk();
        }

        cur = prev->CreateNextChunk();
        if (cur == nullptr) // can't create more!
            return nullptr;

        num_chunks++;
        return cur->Alloc(size);
    }

    bool Free(void *ptr, size_t old_size)
    {
        if (ptr == nullptr || old_size == 0)
            return true;

        ChunkT *cur = head;
        while (cur != nullptr)
        {
            if (cur->Free(ptr, old_size))
            {
                return true;
            }
            cur = cur->NextChunk();
        }
        return false;
    }

    bool BelongsToPool(void *ptr)
    {
        ChunkT *cur = head;
        while (cur != nullptr)
        {
            if (cur->BelongsToChunk(ptr))
            {
                return true;
            }
            cur = cur->NextChunk();
        }
        return false;
    }

    size_t NumChunks() const
    {
        return num_chunks;
    }

private:
    size_t num_chunks;
    ChunkT *head;
};