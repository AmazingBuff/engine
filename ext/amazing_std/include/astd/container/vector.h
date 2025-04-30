#pragma once

#include "astd/base/define.h"
#include "astd/base/trait.h"
#include "astd/base/logger.h"
#include "astd/base/util.h"
#include "astd/memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

template <typename Tp, template <typename> typename Alloc = Allocator>
class Vector
{
    using allocator = Alloc<Tp>;
public:
    class Iterator
    {
    public:
        Iterator() : m_ptr(nullptr) {}
        explicit Iterator(Tp* ptr) : m_ptr(ptr) {}

        Iterator& operator++()
        {
            ++m_ptr;
            return *this;
        }

        Iterator& operator--()
        {
            --m_ptr;
            return *this;
        }

        Tp operator*()
        {
            return *m_ptr;
        }

        Tp* operator->()
        {
            return m_ptr;
        }

        NODISCARD bool operator==(const Iterator& other) const
        {
            return m_ptr == other.m_ptr;
        }

        NODISCARD bool operator!=(const Iterator& other) const
        {
            return m_ptr != other.m_ptr;
        }

    private:
        Tp* m_ptr;
    };
public:
    Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
    explicit Vector(const size_t size) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        reserve(size);
        m_size = size;
    }

    template <typename OtherT>
        requires(std::is_convertible_v<OtherT, Tp>)
    Vector(const std::initializer_list<OtherT>& list) : m_data(nullptr), m_size(list.size()), m_capacity(0)
    {
        reserve(m_size);
        if constexpr (memcopyable<Tp, OtherT>)
            std::memcpy(m_data, list.begin(), sizeof(Tp) * m_size);
        else
        {
            size_t i = 0;
            for (auto& item : list)
            {
                m_data[i] = item;
                i++;
            }
        }
    }

    template <typename OtherT>
        requires(std::is_convertible_v<OtherT, Tp>)
    Vector(const Vector<OtherT>& other) : m_data(nullptr), m_size(other.m_size), m_capacity(other.m_capacity)
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

    template <typename OtherT>
        requires(std::is_convertible_v<OtherT, Tp>)
    Vector& operator=(const Vector<OtherT>& other)
    {
        if (this == &other)
            return *this;

        m_size = other.m_size;
        if (m_capacity < m_size)
            reserve(m_size);

        if constexpr (memcopyable<Tp, OtherT>)
            std::memcpy(m_data, other.m_data, other.m_size * sizeof(Tp));
        else
        {
            for (size_t i = 0; i < m_size; ++i)
                m_data[i] = other.m_data[i];
        }

        return *this;
    }

    template<typename... Args>
        requires(std::is_constructible_v<Tp, Args...>)
    void emplace_back(Args&&... args)
    {
        if (m_size >= m_capacity)
            reserve(m_capacity == 0 ? 4 : (m_capacity * 3 / 2));

        m_data[m_size] = Tp(std::forward<Args>(args)...);
        m_size++;
    }

    void push_back(Tp&& value)
    {
        if (m_size >= m_capacity)
            reserve(m_capacity == 0 ? 4 : (m_capacity * 3 / 2));

        m_data[m_size] = value;
        m_size++;
    }

    void push_back(const Tp& value)
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
        m_size = new_size;
    }

    void swap(Vector& other)
    {
        if (this != &other)
        {
            Amazing::swap(m_data, other.m_data);
            Amazing::swap(m_size, other.m_size);
            Amazing::swap(m_capacity, other.m_capacity);
        }
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

    Iterator begin()
    {
        return Iterator(m_data);
    }

    Iterator const begin() const
    {
        return Iterator(m_data);
    }

    Iterator end()
    {
        return Iterator(m_data + m_size);
    }

    Iterator const end() const
    {
        return Iterator(m_data + m_size);
    }

protected:
    Tp* m_data;
    size_t      m_size;
    size_t      m_capacity;
};


AMAZING_NAMESPACE_END