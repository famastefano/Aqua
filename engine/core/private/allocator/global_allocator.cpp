#include <core/allocator/global_allocator.hpp>
#include <cstdlib>

namespace engine::core
{
void *global_allocator::allocate(u64 byte_count, u64 alignment)
{
    if (alignment <= 16)
        return std::malloc(byte_count);
    return static_cast<u8 *>(std::malloc(byte_count + alignment)) + alignment;
}

void global_allocator::deallocate(void *address, u64 alignment)
{
    if (alignment <= 16)
        return std::free(address);
    std::free(static_cast<u8 *>(address) - alignment);
}

bool global_allocator::can_transfer_ownership_to(allocator &other)
{
    return &other == &get_global_allocator();
}

void global_allocator::transfer_ownership_to(allocator &other)
{
}

global_allocator &global_allocator::instance()
{
    static global_allocator alloc;
    return alloc;
}

allocator &get_global_allocator()
{
    return global_allocator::instance();
}
} // namespace engine::core