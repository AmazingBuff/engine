#pragma once

#include "astd/container/queue.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Tp>
struct RBTreeNode
{
    Tp val;
    RBTreeNode* parent;
    RBTreeNode* left;
    RBTreeNode* right;

    enum class Color : bool
    {
        Red,
        Black
    } color;

    ~RBTreeNode()
    {
        if constexpr (std::is_destructible_v<Tp>)
            val.~Tp();
    }
};

template <typename Tp>
RBTreeNode<Tp>* left_rotate(RBTreeNode<Tp>* node)
{
    RBTreeNode<Tp>* right = node->right;
    node->right = right->left;

    right->left = node;
    right->parent = node->parent;

    if (node->parent)
    {
        if (node == node->parent->left)
            node->parent->left = right;
        else
            node->parent->right = right;
    }

    node->parent = right;

    return right;
}

template <typename Tp>
RBTreeNode<Tp>* right_rotate(RBTreeNode<Tp>* node)
{
    RBTreeNode<Tp>* left = node->left;
    node->left = left->right;

    left->right = node;
    left->parent = node->parent;

    if (node->parent)
    {
        if (node == node->parent->left)
            node->parent->left = left;
        else
            node->parent->right = left;
    }

    node->parent = left;

    return left;
}

template <typename Tp>
void swap(RBTreeNode<Tp>* s1, RBTreeNode<Tp>* s2)
{
    if (s1 == s2)
        return;

    RBTreeNode<Tp>* s1_parent = s1->parent;
    RBTreeNode<Tp>* s2_parent = s2->parent;

    if (s1_parent)
    {
        if (s1 == s1_parent->left)
            s1_parent->left = s2;
        else
            s1_parent->right = s2;
    }

    if (s2_parent)
    {
        if (s2 == s2_parent->left)
            s2_parent->left = s1;
        else
            s2_parent->right = s1;
    }

    s1->parent = s2_parent;
    s2->parent = s1_parent;

    if (s1->left)
        s1->left->parent = s2;
    if (s2->left)
        s2->left->parent = s1;

    if (s1->right)
        s1->right->parent = s2;
    if (s2->right)
        s2->right->parent = s1;

    RBTreeNode<Tp>* left = s1->left;
    s1->left = s2->left;
    s2->left = left;

    RBTreeNode<Tp>* right = s1->right;
    s1->right = s2->right;
    s2->right = right;
}


template <typename Trait>
class RBTree
{
    using key_type = typename Trait::key_type;
    using value_type = typename Trait::value_type;
    using key_compare = typename Trait::key_compare;
    using value_compare = typename Trait::value_compare;
    using node_type = typename Trait::node_type;
    using allocator = typename Trait::allocator;

    static constexpr bool is_set = std::is_same_v<key_type, value_type>;
public:
    class Iterator
    {
    public:
        Iterator() : m_node(nullptr) {}
        explicit Iterator(node_type* node) : m_node(node) {}

        Iterator& operator++()
        {
            if (m_node->right)
            {
                // find the left-most node
                m_node = m_node->right;
                while (m_node->left)
                    m_node = m_node->left;
            }
            else if (m_node->parent)
            {
                node_type* parent = m_node->parent;
                if (parent->left == m_node)
                    m_node = parent;
                else
                    m_node = nullptr;
            }
            else
                m_node = nullptr; // no parent, end

            return *this;
        }

        Iterator& operator--()
        {
            if (m_node->left)
            {
                m_node = m_node->left;
                // find the right-most node
                while (m_node->right)
                    m_node = m_node->right;
            }
            else if (m_node->parent)
            {
                node_type* parent = m_node->parent;
                if (parent->right == m_node)
                    m_node = parent;
                else
                    m_node = nullptr;
            }

            return *this;
        }

        value_type& operator*()
        {
            return m_node->val;
        }

        const value_type& operator*() const
        {
            return m_node->val;
        }

        value_type* operator->()
        {
            return &m_node->val;
        }

        NODISCARD bool operator==(const Iterator& other) const
        {
            return m_node == other.m_node;
        }

        NODISCARD bool operator!=(const Iterator& other) const
        {
            return m_node != other.m_node;
        }
    private:
        node_type* m_node;

        friend class RBTree;
    };

public:
    RBTree() : m_root(nullptr), m_size(0) {}
    ~RBTree()
    {
        clear();
    }

    template <typename... Args>
        requires(std::is_constructible_v<value_type, Args...>)
    void emplace(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        insert(std::move(val));
    }

    template <typename Iter>
    void insert(Iter first, Iter last)
    {
        while (first != last)
        {
            emplace(*first);
            ++first;
        }
    }

    void insert(value_type&& val)
    {
        // find position
        node_type* node = m_root;
        node_type* parent = nullptr;
        bool left_pos = false; // true with left, which false with right
        while (node)
        {
            parent = node;
            if (value_compare()(node->val, val))
            {
                node = node->right;
                left_pos = false;
            }
            else if (value_compare()(val, node->val))
            {
                node = node->left;
                left_pos = true;
            }
            else
            {
                // equal
                if constexpr (!Trait::is_multi)
                    return;
                else
                {
                    // insert to right subtree
                    node = node->right;
                    while (node)
                    {
                        parent = node;
                        node = node->left;
                    }
                }
            }
        }

        node = allocator::allocate(1);
        node->val = val;
        node->parent = parent;
        node->left = nullptr;
        node->right = nullptr;
        node->color = node_type::Color::Red;

        if (m_root)
        {
            if (left_pos)
                parent->left = node;
            else
                parent->right = node;
        }
        else
            m_root = node;

        insert_adjustment(node);

        m_size++;
    }

    void erase(const key_type& key)
    {
        node_type* node = find_node(key);

        if (node == nullptr)
            return;

        erase_adjustment(node);
        m_size--;
    }

    void erase_range(const key_type& key)
    {
        while (node_type* node = find_node(key))
        {
            erase_adjustment(node);
            m_size--;
        }
    }

    void erase(Iterator&& it)
    {
        if (it.m_node)
        {
            erase_adjustment(it.m_node);
            m_size--;
        }
    }

    NODISCARD size_t size() const
    {
        return m_size;
    }

    NODISCARD bool empty() const
    {
        return m_size == 0;
    }

    NODISCARD size_t count(const key_type& key) const
    {
        if (node_type* node = find_node(key))
        {
            if constexpr (!Trait::is_multi)
                return 1;
            else
            {
                Iterator it(node);
                key_type node_key = Trait::key_func(*it);
                uint32_t count = 0;
                while (!key_compare()(node_key, key) && !key_compare()(key, node_key))
                {
                    count++;
                    ++it;
                }
                return count;
            }
        }
        return 0;
    }

    void clear()
    {
        Queue<node_type*> queue;
        if (m_root)
            queue.enqueue(std::move(m_root));
        while (!queue.empty())
        {
            node_type* front = queue.front();
            queue.dequeue();
            if (front->left)
                queue.enqueue(std::move(front->left));
            if (front->right)
                queue.enqueue(std::move(front->right));
            erase_directly(front);
        }

        m_root = nullptr;
        m_size = 0;
    }

    Iterator begin()
    {
        // left most
        node_type* node = m_root;
        if (node)
        {
            while (node->left)
                node = node->left;
        }
        return Iterator(node);
    }

    Iterator end()
    {
        return Iterator();
    }

protected:
    // find node for rb tree or find first node for multi rb tree
    node_type* find_node(const key_type& key) const
    {
        node_type* node = m_root;
        while (node)
        {
            const key_type& node_key = Trait::key_func(node->val);
            if (key_compare()(node_key, key))
                node = node->right;
            else if (key_compare()(key, node_key))
                node = node->left;
            else
            {
                if constexpr (!Trait::is_multi)
                    break;
                else
                {
                    if (node->left)
                        node = node->left;
                    else
                        break;
                }
            }
        }

        return node;
    }

private:
    void erase_directly(node_type* node)
    {
        allocator::deallocate(node);
        node = nullptr;
    }

    // node has been found
    void erase_adjustment(node_type* node)
    {
        if (node->left != nullptr && node->right != nullptr)
        {
            // two children
            node_type* left_most = node->right;
            while (left_most->left)
                left_most = left_most->left;

            swap(node, left_most);
            erase_adjustment(node);
        }
        else if (node->left != nullptr || node->right != nullptr)
        {
            // one child, this child must be red
            node_type* child = node->left ? node->left : node->right;
            child->parent = node->parent;
            child->color = node_type::Color::Black;
            if (node == m_root)
            {
                erase_directly(node);
                m_root = child;
            }
            else
            {
                node_type* parent = node->parent;
                if (node == parent->left)
                    parent->left = child;
                else
                    parent->right = child;
                erase_directly(node);
            }
        }
        else
        {
            // no child
            if (node == m_root)
            {
                erase_directly(node);
                m_root = nullptr;
            }
            else
            {
                node_type* parent = node->parent;
                bool left_pos = false;
                if (node == parent->left)
                    left_pos = true;
                else
                    left_pos = false;

                if (node->color == node_type::Color::Red)
                {
                    erase_directly(node);

                    if (left_pos)
                        parent->left = nullptr;
                    else
                        parent->right = nullptr;
                }
                else
                {
                    // if node is black, it must have a brother exclude root
                    erase_directly(node);

                    node_type* brother = nullptr;
                    if (left_pos)
                        brother = parent->right;
                    else
                        brother = parent->left;

                    if (brother->left != nullptr && brother->right != nullptr)
                    {
                        if (brother->color == node_type::Color::Black)
                        {
                            // both children of brother must be red
                            brother->color = parent->color;
                            parent->color = node_type::Color::Black;
                            if (left_pos)
                            {
                                // brother is right
                                brother->right->color = node_type::Color::Black;
                                left_rotate(parent);
                            }
                            else
                            {
                                // brother is left
                                brother->left->color = node_type::Color::Black;
                                right_rotate(parent);
                            }
                        }
                        else
                        {
                            // both children of brother must be black, parent must be black
                            if (left_pos)
                            {
                                // brother is right
                                brother->left->color = node_type::Color::Red;
                                left_rotate(parent);
                            }
                            else
                            {
                                // brother is left
                                brother->right->color = node_type::Color::Red;
                                right_rotate(parent);
                            }
                        }
                    }
                    else if (brother->left != nullptr)
                    {
                        if (left_pos)
                        {
                            // brother is right, while the child of brother is left
                            brother->color = node_type::Color::Red;
                            brother->left->color = node_type::Color::Black;
                            brother = right_rotate(brother);

                            brother->color = parent->color;
                            parent->color = node_type::Color::Black;
                            brother->right->color = node_type::Color::Black;
                            left_rotate(parent);
                        }
                        else
                        {
                            // both brother and the child of brother are left
                            brother->color = parent->color;
                            parent->color = node_type::Color::Black;
                            brother->left->color = node_type::Color::Black;
                            right_rotate(parent);
                        }
                    }
                    else if (brother->right != nullptr)
                    {
                        if (left_pos)
                        {
                            // both brother and the child of brother are right
                            brother->color = parent->color;
                            parent->color = node_type::Color::Black;
                            brother->right->color = node_type::Color::Black;
                            left_rotate(parent);
                        }
                        else
                        {
                            // brother is left, while the child of brother is right
                            brother->color = node_type::Color::Red;
                            brother->right->color = node_type::Color::Black;
                            brother = left_rotate(brother);

                            brother->color = parent->color;
                            parent->color = node_type::Color::Black;
                            brother->left->color = node_type::Color::Black;
                            left_rotate(parent);
                        }
                    }
                    else
                    {
                        // brother has no child, so the color of it must be black
                        brother->color = node_type::Color::Red;
                        parent->color = node_type::Color::Black;
                    }
                }
            }
        }
    }


    // node has been inserted to appropriate position
    void insert_adjustment(node_type* node)
    {
        if (node == m_root)
        {
            node->color = node_type::Color::Black;
            return;
        }

        node_type* parent = node->parent;
        if (parent->color == node_type::Color::Red)
        {
            // root node can't be red, so parent can't be root, which means parent must have parent
            node_type* grand = parent->parent;

            // parent red while uncle black
            if (grand->left == nullptr || grand->right == nullptr ||
                grand->left->color == node_type::Color::Black ||
                grand->right->color == node_type::Color::Black)
            {
                node_type* adjust = nullptr;
                if (parent->left == node)
                {
                    if (grand->left == parent)
                    {
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = right_rotate(grand);
                    }
                    else
                    {
                        parent = right_rotate(parent);
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = left_rotate(grand);
                    }
                }
                else
                {
                    if (grand->left == parent)
                    {
                        parent = left_rotate(parent);
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = right_rotate(grand);
                    }
                    else
                    {
                        grand->color = parent->color;
                        parent->color = node_type::Color::Black;
                        adjust = left_rotate(grand);
                    }
                }

                if (adjust->parent == nullptr)
                    m_root = adjust;
            }
            else
            {
                // both parent and uncle are red
                grand->left->color = node_type::Color::Black;
                grand->right->color = node_type::Color::Black;
                grand->color = node_type::Color::Red;

                insert_adjustment(grand);
            }
        }
    }

private:
    node_type* m_root;
    size_t      m_size;
};

INTERNAL_NAMESPACE_END

AMAZING_NAMESPACE_END