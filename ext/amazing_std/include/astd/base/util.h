#pragma once

#include <type_traits>
#include "macro.h"

AMAZING_NAMESPACE_BEGIN

template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr Tp align_to(const Tp value, const Tp alignment)
{
    return (value + alignment - 1) / alignment * alignment;
}


inline constexpr size_t hash_str(const char* str, size_t hash = 2166136261ull)
{
    return (*str == '\0') ? hash : hash_str(str + 1, (hash ^ static_cast<unsigned int>(*str)) * 16777619);
}


template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr size_t hash_combine(const size_t seed, Tp val)
{
    const size_t value = static_cast<size_t>(val);
    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

AMAZING_NAMESPACE_END