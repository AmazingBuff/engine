#pragma once

#include <type_traits>
#include "macro.h"

AMAZING_NAMESPACE_BEGIN

template<typename Tp>
concept copyable = std::is_trivially_copyable_v<Tp> && std::is_standard_layout_v<Tp>;

template<typename Tp, typename Up>
concept memcopyable = copyable<Tp> && copyable<Up> && std::is_same_v<Tp, Up>;

AMAZING_NAMESPACE_END