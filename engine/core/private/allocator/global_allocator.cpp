#include <core/allocator/global_allocator.hpp>
#include <cstdlib>

namespace engine::core
{
void *global_allocator::allocate(u64 byte_count, u64 alignment)
{
    u64 const size   = byte_count + alignment <= 16 ? 0 : alignment;
    u64 const stride = alignment <= 16 ? 0 : alignment;
    if (void *mem = std::malloc(size))
        return static_cast<u8 *>(mem) + stride;
    return get_null_allocator().allocate(byte_count, alignment);
}

void global_allocator::deallocate(void *address, u64 alignment)
{
    if (!address)
        return;

    u64 const stride     = alignment <= 16 ? 0 : alignment;
    u8 *original_address = static_cast<u8 *>(address) - stride;
    std::free(original_address);
}

bool global_allocator::can_transfer_ownership_to(allocator &other)
{
    return &other == &get_global_allocator();
}

void global_allocator::transfer_ownership_to(allocator &other)
{
    if (&other != &get_global_allocator())
        std::abort();
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