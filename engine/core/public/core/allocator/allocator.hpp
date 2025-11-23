#pragma once

#include <core/types.hpp>

namespace engine::core
{
class allocator
{
  public:
    virtual void *allocate(u64 byte_count, u64 alignment) = 0;
    virtual void deallocate(void *address, u64 alignment) = 0;
    virtual bool can_transfer_ownership_to(allocator &)
    {
        return false;
    }
    virtual void transfer_ownership_to(allocator &)
    {
    }
};

allocator &get_global_allocator();
allocator &get_null_allocator();
} // namespace engine::core
