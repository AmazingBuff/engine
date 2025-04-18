//
// Created by AmazingBuff on 2025/4/3.
//

#pragma once

#include "vector.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Tp, template <typename> typename Alloc = Allocator>
class StringT : public Vector<Tp, Alloc>
{
    using Str = Vector<Tp, Alloc>;
    using allocator = Alloc<Tp>;
    using allocator_next = Alloc<size_t>;
public:
    StringT() = default;
    StringT(const Tp* str)
    {
        Str::m_size = strlen(str);
        Str::m_capacity = Str::m_size;
        Str::m_data = allocator::allocate(Str::m_capacity);
        std::memcpy(Str::m_data, str, Str::m_size * sizeof(Tp));
    }

    NODISCARD size_t find(const Tp* str) const
    {
        size_t size = std::strlen(str);
        return find(str, size);
    }

    NODISCARD size_t find(const StringT& str) const
    {
        return find(str.c_str(), str.m_size);
    }

    NODISCARD const Tp* c_str() const
    {
        return Str::m_data;
    }

    NODISCARD StringT substr(size_t pos, size_t count) const
    {
        StringT ret;
        if (pos < Str::m_size)
        {
            size_t len = count;
            if (pos + count > Str::m_size)
                len = Str::m_size - pos;
            std::memcpy(ret.m_data, Str::m_data + pos, len * sizeof(Tp));
        }

        return ret;
    }

    NODISCARD StringT operator+(const StringT& other) const
    {
        StringT str;
        str.resize(Str::m_size + other.m_size);
        std::memcpy(str.m_data, Str::m_data, Str::m_size * sizeof(Tp));
        std::memcpy(str.m_data + Str::m_size, other.m_data, other.m_size * sizeof(Tp));
        return str;
    }

    StringT& operator+=(const StringT& other)
    {
        if (Str::m_size + other.m_size > Str::m_capacity)
            Str::reserve(Str::m_size + other.m_size);
        std::memcpy(Str::m_data + Str::m_size, other.m_data, other.m_size * sizeof(Tp));
        Str::m_size += other.m_size;
        return *this;
    }

    NODISCARD bool operator==(const StringT& other) const
    {
        if (Str::m_size != other.m_size)
            return false;
        for (size_t i = 0; i < Str::m_size; i++)
        {
            if (Str::m_data[i] != Str::m_data[i])
                return false;
        }
        return true;
    }

    NODISCARD bool operator!=(const StringT& other) const
    {
        return !(*this == other);
    }

public:
    static constexpr size_t end = std::numeric_limits<size_t>::max();

private:
    size_t find(const Tp* str, size_t size) const
    {
        if (size > Str::m_size || size <= 0)
            return end;

        // next array
        size_t len = 0;

        Vector<size_t> next(size);
        next[0] = len;

        size_t index = 1;
        while (index < size)
        {
            if (str[len] == str[index])
            {
                ++len;
                next[index] = len;
                index++;
            }
            else
            {
                if (len == 0)
                {
                    next[index] = len;
                    index++;
                }
                else
                    len = next[len - 1];
            }
        }

        // kmp
        size_t i = 0, j = 0;
        while (i < Str::m_size && j < size)
        {
            if (Str::m_data[i] == str[j])
            {
                i++;
                j++;
            }
            else
            {
                if (j == 0)
                    i++;
                else
                    j = next[j - 1];
            }
        }

        return j == size ? i - j : end;
    }
};

INTERNAL_NAMESPACE_END

using String = Internal::StringT<char>;
using WString = Internal::StringT<wchar_t>;

AMAZING_NAMESPACE_END