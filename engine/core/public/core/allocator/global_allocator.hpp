#pragma once

#include <core/allocator/allocator.hpp>

namespace engine::core
{
class global_allocator : public allocator
{
  private:
    global_allocator() = default;

  public:
    void *allocate(u64 byte_count, u64 alignment) override;
    void deallocate(void *address, u64 alignment) override;
    bool can_transfer_ownership_to(allocator &other) override;
    void transfer_ownership_to(allocator& other) override;

    static global_allocator& instance();
};
} // namespace engine::core