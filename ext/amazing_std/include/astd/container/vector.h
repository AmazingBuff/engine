#pragma once

#include "base/define.h"
#include "base/trait.h"
#include "base/logger.h"
#include "memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

template <typename Tp, template <typename> typename Alloc = Allocator>
class Vector
{
    using allocator = Alloc<Tp>;
public:
    Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
    explicit Vector(const size_t size) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        reserve(size);
    }

    template <typename OtherT>
        requires(std::is_convertible_v<OtherT, Tp>)
    explicit Vector(const Vector<OtherT>& other) : m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
    {
        reserve(m_capacity);
        if constexpr (memcopyable<Tp, OtherT>)
            std::memcpy(m_data, other.m_data, other.m_size * sizeof(Tp));
        else
        {
            for (size_t i = 0; i < m_size; ++i)
                m_data[i] = other.m_data[i];
        }
    }

    ~Vector()
    {
        if (m_data != nullptr)
            allocator::deallocate(m_data);
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    void push_back(Tp&& value)
    {
        if (m_size >= m_capacity)
            reserve(m_capacity == 0 ? 4 : (m_capacity * 3 / 2));

        m_data[m_size] = value;
        m_size++;
    }

    void pop_back()
    {
        if (!empty())
            m_size--;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD size_t capacity() const
    {
        return m_capacity;
    }

    NODISCARD const Tp* data() const
    {
        return m_data;
    }

    NODISCARD Tp* data()
    {
        return m_data;
    }

    void reserve(const size_t new_capacity)
    {
        if (m_capacity == new_capacity)
            return;

        if (new_capacity != 0)
        {
            Tp* new_data = allocator::allocate(m_data, new_capacity);
            if (new_data != m_data)
            {
                if constexpr (copyable<Tp>)
                    std::memcpy(new_data, m_data, sizeof(Tp) * m_capacity);
                else
                {
                    for (size_t i = 0; i < m_capacity; i++)
                        new_data[i] = m_data[i];
                }
                allocator::deallocate(m_data);
                m_data = new_data;
            }
        }
        else
        {
            allocator::deallocate(m_data);
            m_data = nullptr;
        }

        m_capacity = new_capacity;

        if (m_size > m_capacity)
            m_size = m_capacity;
    }

    void resize(const size_t new_size)
    {
        if (new_size > m_capacity)
            reserve(new_size);
        else
            m_size = new_size;
    }

    void clear()
    {
        m_size = 0;
    }

    NODISCARD Tp& operator[](size_t index)
    {
        CONTAINER_ASSERT(index < m_size, "index out of range");
        return m_data[index];
    }

    NODISCARD const Tp& operator[](size_t index) const
    {
        CONTAINER_ASSERT(index < m_size, "index out of range");
        return m_data[index];
    }

protected:
    Tp*         m_data;
    size_t      m_size;
    size_t      m_capacity;
};


AMAZING_NAMESPACE_END