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
    Edge const& min() const { return m_min; }
    Edge const& max() const { return m_max; }

    Interval& merge(const Interval& rhs);
    Interval& intersect(const Interval& rhs);
private:
    Edge m_min;
    Edge m_max;
};

template <typename Edge>
bool merge(const Interval<Edge>& lhs, const Interval<Edge>& rhs, Interval<Edge>& r);

template <typename Edge>
bool intersect(const Interval<Edge>& lhs, const Interval<Edge>& rhs, Interval<Edge>& r);

#include "misc.inl"
AMAZING_NAMESPACE_END
#endif //MISC_H
