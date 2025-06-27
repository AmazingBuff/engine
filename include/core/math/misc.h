//
// Created by AmazingBuff on 2025/6/24.
//

#ifndef MISC_H
#define MISC_H

#include "algebra.h"

AMAZING_NAMESPACE_BEGIN

template <typename Edge>
class Interval
{
public:
    Interval() : m_min{}, m_max{} {}
    Interval(const Edge& l, const Edge& r) : m_min(l), m_max(r) {}
    Edge& min() { return m_min; }
    Edge& max() { return m_max; }

    Interval& merge(const Interval& rhs);
    Interval& intersect(const Interval& rhs);

    friend bool merge(const Interval& lhs, const Interval& rhs, Interval& r);
    friend bool intersect(const Interval& lhs, const Interval& rhs, Interval& r);

private:
    Edge m_min;
    Edge m_max;
};

#include "misc.inl"
AMAZING_NAMESPACE_END
#endif //MISC_H
