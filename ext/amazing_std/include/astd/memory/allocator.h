#pragma once

#include "astd/base/define.h"

AMAZING_NAMESPACE_BEGIN

constexpr static size_t k_global_memory_size = 256 * 1024 * 1024;   // 256 MB
constexpr static size_t k_local_memory_size = 64 * 1024 * 1024;    // 64 MB
constexpr static size_t k_cache_alignment = 64;


void* allocate(size_t size, size_t alignment = k_cache_alignment, void* data = nullptr);

// recompute
void* reallocate(void* p, size_t size, size_t alignment = k_cache_alignment, void* data = nullptr);

void deallocate(void* p);

template<typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
Tp* allocate(size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
{
    return static_cast<Tp*>(allocate(sizeof(Tp) * count, alignment, data));
}

template<typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
Tp* reallocate(void* p, size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
{
    return static_cast<Tp*>(reallocate(p, sizeof(Tp) * count, alignment, data));
}


template <typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
class Allocator
{
public:
    static Tp* allocate(size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
    {
        return Amazing::allocate<Tp>(count, alignment, data);
    }

    // allocate memory near p with count
    static Tp* reallocate(void* p, size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
    {
        return Amazing::reallocate<Tp>(p, count, alignment, data);
    }

    static void deallocate(Tp* p)
    {
        if (p == nullptr)
            return;

        if constexpr (std::is_destructible_v<Tp>)
            p->~Tp();

        Amazing::deallocate(p);
    }
};


#define PLACEMENT_NEW(type, size, ...) (new (allocate(size)) type(__VA_ARGS__))
#define PLACEMENT_DELETE(type, p) if constexpr (std::is_destructible_v<type>) if (p) p->~type(); deallocate(p)
#define STACK_NEW(type, count) static_cast<type*>(alloca(count * sizeof(type)))

AMAZING_NAMESPACE_END