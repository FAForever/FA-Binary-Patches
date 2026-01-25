#pragma once
#include <cstdint>

struct AllocationInfo
{
    size_t total_storage;
    size_t occupied_storage;
    size_t chunk_count;
};