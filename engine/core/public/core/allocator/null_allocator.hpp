#pragma once

#include <core/allocator/allocator.hpp>

namespace engine::core
{
class null_allocator : public allocator
{
  private:
  null_allocator() = default;

  public:
    void *allocate(u64, u64) override;
    void deallocate(void *, u64) override;

    static null_allocator& instance();
};
} // namespace engine::core
