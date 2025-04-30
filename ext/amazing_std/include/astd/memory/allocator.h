#pragma once

#include "astd/base/define.h"
#include <mutex>

AMAZING_NAMESPACE_BEGIN

constexpr static size_t k_global_memory_size = 256 * 1024 * 1024;   // 256 MB
constexpr static size_t k_local_memory_size = 256 * 1024;           // 256 KB


void* allocate(size_t size, void* data);

// recompute
void* allocate(void* p, size_t size, void* data);

void deallocate(void* p);

template<typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
Tp* allocate(size_t count, void* data)
{
    return static_cast<Tp*>(allocate(sizeof(Tp) * count, data));
}

template<typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
Tp* allocate(void* p, size_t count, void* data)
{
    return static_cast<Tp*>(allocate(p, sizeof(Tp) * count, data));
}


template <typename Tp>
    requires(!std::has_virtual_destructor_v<Tp>)
class Allocator
{
public:
    static Tp* allocate(size_t count, void* data = nullptr)
    {
        return Amazing::allocate<Tp>(count, data);
    }

    // allocate memory near p with count
    static Tp* allocate(void* p, size_t count, void* data = nullptr)
    {
        return Amazing::allocate<Tp>(p, count, data);
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


#define PLACEMENT_NEW(type, size, data, ...) (new (allocate(size, data)) type(__VA_ARGS__))
#define PLACEMENT_DELETE(type, p) if (p && std::is_destructible_v<type>) p->~type(); deallocate(p)

AMAZING_NAMESPACE_END