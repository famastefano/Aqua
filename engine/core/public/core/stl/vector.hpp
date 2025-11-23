#pragma once

#include <algorithm>
#include <bit>
#include <cmath>
#include <core/allocator/allocator.hpp>
#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace engine::core
{
template <typename T> class vector
{
  protected:
    struct alignas(char) header
    {
        u32 size{};
        u32 capacity{};
    };
    T *mem{};
    allocator &alloc{};

    constexpr static header const &empty_header()
    {
        static header h{};
        return h;
    }

    constexpr static u32 get_header_stride()
    {
        static_assert(std::has_single_bit(sizeof(header)));
        if constexpr (alignof(T) <= sizeof(header))
            return sizeof(header);
        return alignof(T);
    }

    static constexpr u32 calculate_alloc_size(u32 size)
    {
        return get_header_stride() + sizeof(T) * size;
    }

    static constexpr u32 calculate_new_capacity(u32 current)
    {
        return std::ceilf(float(u32) * 1.5f);
    }

    constexpr auto &get_header()
    {
        if (mem)
        {
            header *ptr = static_cast<header *>(static_cast<u64>(mem) - get_header_stride());
            return *ptr;
        }
        return empty_header();
    }

    constexpr void do_realloc(u32 new_capacity)
    {
        void *new_alloc_block = alloc->allocate(calculate_alloc_size(new_capacity), alignof(T));
        auto &new_header      = *new (new_alloc_block) header();
        new_header.size       = size();
        new_header.capacity   = new_capacity;
        T *new_mem            = static_cast<u8 *>(new_alloc_block) + get_header_stride();
        std::copy(begin(), end(), new_mem);
        clear();
        mem = new_mem;
    }

  public:
    constexpr vector() : vector(get_global_allocator())
    {
    }
    constexpr vector(allocator &alloc) : alloc(alloc)
    {
    }
    constexpr explicit vector(u32 size, allocator &alloc = get_global_allocator()) : vector(alloc)
    {
        resize(size);
    }
    constexpr explicit vector(u32 size, T const &value, allocator &alloc = get_global_allocator()) : vector(alloc)
    {
        resize(size, value);
    }
    constexpr explicit vector(std::initializer_list<T> init) : vector(init.begin(), init.end())
    {
    }

    template <std::input_iterator TIterator>
    constexpr explicit vector(TIterator from, TIterator to, allocator &alloc = get_global_allocator()) : vector(alloc)
    {
        if constexpr (std::random_access_iterator<TIterator>)
        {
            u32 const sz = to - from;
            if (sz)
            {
                do_realloc(sz);
                for (u32 i = 0; i < sz; ++i)
                    new (mem + i) T(from[sz]);
                get_header().size = sz;
            }
        }
        else
        {
            while (from != to)
                emplace_back(*from++);
        }
    }

    constexpr vector(vector const &other, allocator &alloc = get_global_allocator())
        : vector(other.begin(), other.end(), alloc)
    {
    }
    constexpr vector(vector &&other, allocator &alloc = get_global_allocator()) : vector(alloc)
    {
        *this = std::move(other);
    }
    constexpr ~vector()
    {
        clear();
    }
    constexpr vector &operator=(vector const &other)
    {
        if (&other == this)
            return;
        insert(end(), other.begin(), other.end());
    }
    constexpr vector &operator=(vector &&other)
    {
        if (other.alloc.can_transfer_ownership_to(alloc))
        {
            other.alloc.transfer_ownership_to(alloc);
            mem = std::exchange(other.mem, nullptr);
        }
        else
        {
            std::abort();
        }
    }

    constexpr auto &allocator()
    {
        return alloc;
    }

    constexpr auto &operator[](u32 pos)
    {
        if (pos >= size())
            std::abort();
        return mem[pos];
    }
    constexpr auto &front()
    {
        if (empty())
            std::abort();
        return *mem;
    }

    constexpr auto &back()
    {
        if (empty())
            std::abort();
        return *(mem + size() - 1);
    }

    constexpr auto begin()
    {
        return mem;
    }

    constexpr auto end()
    {
        return mem + size();
    }

    constexpr auto rbegin()
    {
        std::abort();
    }

    constexpr auto rend()
    {
        std::abort();
    }

    constexpr bool empty() const
    {
        return size() > 0;
    }

    constexpr u32 size() const
    {
        return get_header().size;
    }

    constexpr u32 capacity() const
    {
        return get_header().capacity;
    }

    constexpr void reserve(u32 new_capacity)
    {
        if (capacity() < new_capacity)
            do_realloc(new_capacity);
    }

    constexpr void clear()
    {
        if (mem)
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                for (auto &item : *this)
                    item->~T();
            }
            get_header().size = 0;
        }
    }

    template <typename... TArgs> constexpr auto &emplace(u32 pos, TArgs &&...args)
    {
        return emplace(begin() + pos, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs> constexpr auto &emplace(T *pos, TArgs &&...args)
    {
        if (pos == end())
        {
            u32 const new_capacity = empty() ? 1 : calculate_new_capacity(capacity());
            do_realloc(new_capacity);
            get_header().size += 1;
            new (end() - 1) T(std::forward<TArgs>(args)...);
        }
        else
        {
            std::abort();
        }
    }

    template <typename... TArgs> constexpr auto &emplace_back(TArgs &&...args)
    {
        return emplace(end(), std::forward<TArgs>(args)...);
    }

    template <typename... TArgs> constexpr auto &push_back(TArgs &&...args)
    {
        return emplace_back(std::forward<TArgs>(args)...);
    }

    constexpr auto erase(u32 pos)
    {
        return erase(pos, 1);
    }

    constexpr auto erase(u32 pos, u32 count)
    {
        auto const from = begin() + pos;
        auto const to   = from + count;
        return erase(from, to);
    }

    constexpr auto erase(T *pos)
    {
        return erase(pos, pos + 1);
    }

    constexpr auto erase(T *from, T *to)
    {
        std::abort();
    }

    constexpr void resize(u32 new_size)
    {
        if (new_size > 0)
        {
            do_realloc(new_size);
            for (u32 i = 0; i < new_size; ++i)
                new (mem + i) T();
            get_header().size = new_size;
        }
        else
        {
            clear();
        }
    }

    constexpr void resize(u32 new_size, T const &value)
    {
        if (new_size > 0)
        {
            do_realloc(new_size);
            for (u32 i = 0; i < new_size; ++i)
                new (mem + i) T(value);
            get_header().size = new_size;
        }
        else
        {
            clear();
        }
    }

    constexpr void swap(vector &other)
    {
        vector cloned_me(*this);
        *this = std::move(other);
        other = std::move(cloned_me);
    }
};
} // namespace engine::core