#pragma once

#include "tree.h"

AMAZING_NAMESPACE_BEGIN

INTERNAL_NAMESPACE_BEGIN

template <typename Tp, typename Pred, template <typename> typename Alloc, bool Multi>
class SetTrait
{
public:
    using key_type = Tp;
    using value_type = Tp;
    using key_compare = Pred;
    using node_type = RBTreeNode<value_type>;
    using allocator = Alloc<node_type>;
    using value_compare = key_compare;

    static constexpr bool is_multi = Multi;

    // extract key from element value
    static const key_type& key_func(const value_type& val)
    {
        return val;
    }
};

INTERNAL_NAMESPACE_END

template <typename Tp, typename Pred = Less<Tp>, template <typename> typename Alloc = Allocator>
class Set : public Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, false>>
{
    using Tree = Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, false>>;
public:

};

template <typename Tp, typename Pred = Less<Tp>, template <typename> typename Alloc = Allocator>
class MultiSet : public Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, true>>
{
    using Tree = Internal::RBTree<Internal::SetTrait<Tp, Pred, Alloc, true>>;
public:
    // todo: support multiset
};


AMAZING_NAMESPACE_END