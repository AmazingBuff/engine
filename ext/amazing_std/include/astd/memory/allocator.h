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
Tp* allocate(size_t count, void* data)
{
    return static_cast<Tp*>(allocate(sizeof(Tp) * count, data));
}

template<typename Tp>
Tp* allocate(void* p, size_t count, void* data)
{
    return static_cast<Tp*>(allocate(p, sizeof(Tp) * count, data));
}


template <typename Tp>
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
        if constexpr (std::is_destructible_v<Tp>)
            p->~Tp();

        Amazing::deallocate(p);
    }
};

AMAZING_NAMESPACE_END