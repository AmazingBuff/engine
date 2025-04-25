#pragma once

#include "tree.h"

AMAZING_NAMESPACE_BEGIN
INTERNAL_NAMESPACE_BEGIN

template <typename Key, typename Tp, typename Pred, template <typename> typename Alloc, bool Multi>
class MapTrait
{
public:
    using key_type      =   Key;
    using value_type    =   Pair<Key, Tp>;
    using key_compare   =   Pred;
    using node_type     =   RBTreeNode<value_type>;
    using allocator     =   Alloc<node_type>;
    
    class value_compare
    {
    public:
        NODISCARD bool operator()(const value_type& lhs, const value_type& rhs) const 
        {
            return key_compare()(lhs.first, rhs.first);
        }
    };

    static constexpr bool is_multi = Multi;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val.first;
    }
};

INTERNAL_NAMESPACE_END

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class Map : public Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>
{
    using Tree = Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, false>>;
    using Iterator = typename Tree::Iterator;
public:
    Tp& operator[](const Key& key)
    {
        auto node = Tree::find_node(key);
        if (node == nullptr)
        {
            Tree::emplace(key, Tp());
            return Tree::find_node(key)->val.second;
        }

        return node->val.second;
    }

    const Tp& operator[](const Key& key) const
    {
        return Tree::find_node(key)->val.second;
    }

    Iterator find(const Key& key)
    {
        return Iterator(Tree::find_node(key));
    }

    Iterator const find(const Key& key) const
    {
        return Iterator(Tree::find_node(key));
    }
};

template <typename Key, typename Tp, typename Pred = Less<Key>, template <typename> typename Alloc = Allocator>
class MultiMap : public Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>
{
    using Tree = Internal::RBTree<Internal::MapTrait<Key, Tp, Pred, Alloc, true>>;
public:
    // todo: support multimap
};


AMAZING_NAMESPACE_END