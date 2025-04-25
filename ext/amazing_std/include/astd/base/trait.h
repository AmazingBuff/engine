#pragma once

#include <type_traits>
#include "macro.h"

AMAZING_NAMESPACE_BEGIN

template<typename Tp>
concept copyable = std::is_trivially_copyable_v<Tp> && std::is_standard_layout_v<Tp>;

template<typename Tp, typename Up>
concept memcopyable = copyable<Tp> && copyable<Up> && std::is_same_v<Tp, Up>;


template<typename T>
    requires std::is_enum_v<T>
class Enum
{
    using type = std::underlying_type_t<T>;
public:
    Enum();
    explicit Enum(const type& value);
    explicit Enum(const T& value);
    Enum(const Enum& other);
    Enum(Enum&& other) noexcept;

    // compare
    NODISCARD constexpr bool operator==(const Enum& value) const;
    NODISCARD constexpr bool operator!=(const Enum& value) const;
    NODISCARD constexpr bool operator>(const Enum& value) const;
    NODISCARD constexpr bool operator>=(const Enum& value) const;
    NODISCARD constexpr bool operator<(const Enum& value) const;
    NODISCARD constexpr bool operator<=(const Enum& value) const;

    Enum& operator=(const T& value);
    Enum& operator=(const Enum& value);

    NODISCARD constexpr bool operator==(const T& value) const;
    NODISCARD constexpr bool operator!=(const T& value) const;
    NODISCARD constexpr bool operator>(const T& value) const;
    NODISCARD constexpr bool operator>=(const T& value) const;
    NODISCARD constexpr bool operator<(const T& value) const;
    NODISCARD constexpr bool operator<=(const T& value) const;

    // transform
    NODISCARD constexpr explicit operator bool() const;
    NODISCARD constexpr explicit operator T() const;
    NODISCARD constexpr explicit operator type() const;
protected:
    type m_value;
};

template<typename T>
    requires std::is_enum_v<T>
class BitFlag : public Enum<T>
{
    using type = std::underlying_type_t<T>;
public:
    BitFlag() = default;
    explicit BitFlag(const type& value);
    explicit BitFlag(const T& value);
    BitFlag(const BitFlag& other);
    BitFlag(BitFlag&& other) noexcept;

    // bit op
    constexpr BitFlag& operator|=(const BitFlag& value);
    constexpr BitFlag& operator&=(const BitFlag& value);
    constexpr BitFlag& operator^=(const BitFlag& value);
    NODISCARD constexpr BitFlag operator|(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator&(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator^(const BitFlag& value) const;
    NODISCARD constexpr BitFlag operator|(const T& value) const;
    NODISCARD constexpr BitFlag operator&(const T& value) const;
    NODISCARD constexpr BitFlag operator^(const T& value) const;
    NODISCARD constexpr BitFlag operator~() const;

    BitFlag& operator=(const BitFlag& value);
    BitFlag& operator=(const T& value);
};

template<typename Tp>
NODISCARD constexpr std::underlying_type_t<Tp> to_underlying(BitFlag<Tp> val)
{
    return static_cast<std::underlying_type_t<Tp>>(val);
}

template<typename Tp>
NODISCARD constexpr std::underlying_type_t<Tp> to_underlying(Tp val)
{
    return static_cast<std::underlying_type_t<Tp>>(val);
}

#include "trait.inl"

AMAZING_NAMESPACE_END