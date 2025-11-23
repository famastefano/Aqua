#include <core/allocator/null_allocator.hpp>
#include <cstdlib>

namespace engine::core
{
void *null_allocator::allocate(u64, u64)
{
    std::abort();
}

void null_allocator::deallocate(void *, u64)
{
    std::abort();
}

null_allocator &null_allocator::instance()
{
    static null_allocator alloc;
    return alloc;
}

allocator &get_null_allocator()
{
    return null_allocator::instance();
}
} // namespace engine::core
