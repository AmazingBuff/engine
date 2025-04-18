#pragma once

#include "base/define.h"
#include "base/trait.h"
#include "memory/allocator.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template<typename Tp>
struct ListNode
{
    Tp val;
    ListNode* prev;
    ListNode* next;
};

INTERNAL_NAMESPACE_END


template<typename Tp, template <typename> typename Alloc = Allocator>
class List
{
    using allocator = Alloc<Internal::ListNode<Tp>>;
public:
    List() : m_size(0)
    {
        m_head = allocator::allocate(1);
        m_head->val = Tp();
        m_head->prev = nullptr;
        m_head->next = nullptr;
    }

    ~List()
    {
        clear();

        allocator::deallocate(m_head);
        m_head = nullptr;
    }

    void push_back(Tp&& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->next = nullptr;

        if (!empty())
        {
            m_head->prev->next = node;
            node->prev = m_head->prev;
        }
        else
        {
            m_head->next = node;
            node->prev = m_head;
        }

        m_head->prev = node;
        m_size++;
    }

    void push_front(Tp&& value)
    {
        Internal::ListNode<Tp>* node = allocator::allocate(1);
        node->val = value;
        node->prev = m_head;

        if (!empty())
        {
            m_head->next->front = node;
            node->next = m_head->next;
        }
        else
        {
            m_head->next = node;
            node->next = nullptr;
        }

        m_head->next = node;
        m_size++;
    }

    void pop_back()
    {
        if (empty())
            return;

        Internal::ListNode<Tp>* node = m_head->prev;

        if (m_size > 1)
            m_head->prev = node->prev;
        else
        {
            m_head->prev = nullptr;
            m_head->next = nullptr;
        }
        
        allocator::deallocate(node);
        node = nullptr;

        m_size--;
    }

    void pop_front()
    {
        if (empty())
            return;

        Internal::ListNode<Tp>* node = m_head->next;

        if (m_size > 1)
            m_head->next = node->next;
        else
        {
            m_head->prev = nullptr;
            m_head->next = nullptr;
        }

        allocator::deallocate(node);
        node = nullptr;

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

    NODISCARD Tp front() const
    {
        return m_head->next->val;
    }

    NODISCARD Tp back() const
    {
        return m_head->prev->val;
    }

    void clear()
    {
        Internal::ListNode<Tp>* node = m_head->next;
        while (node != nullptr)
        {
            Internal::ListNode<Tp>* next = node->next;

            allocator::deallocate(node);
            node = next;
        }
        m_head->next = nullptr;

        m_size = 0;
    }

private:
    Internal::ListNode<Tp>* m_head;
    size_t m_size;
};


AMAZING_NAMESPACE_END