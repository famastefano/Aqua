#pragma once

#include <core/types.hpp>

namespace engine::core
{
    class allocator
    {
    public:
        virtual void* allocate(u64 byte_count, u64 alignment) = 0;
        virtual void deallocate(void* address, u64 alignment) = 0;
    };
}
