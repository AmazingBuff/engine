

template <typename Edge>
Interval<Edge>& Interval<Edge>::merge(const Interval& rhs)
{
    m_min = std::min(m_min, rhs.m_min);
    m_max = std::max(m_max, rhs.m_max);
    return *this;
}

template <typename Edge>
Interval<Edge>& Interval<Edge>::intersect(const Interval& rhs)
{
    if (rhs.m_max <= m_max && rhs.m_max >= m_min)
    {
        m_min = std::max(m_min, rhs.m_min);
        m_max = std::min(m_max, rhs.m_max);
    }

    return *this;
}

template <typename Edge>
bool merge(const Interval<Edge>& lhs, const Interval<Edge>& rhs, Interval<Edge>& r)
{
    Edge min = std::min(lhs.min(), rhs.min());
    Edge max = std::max(lhs.max(), rhs.max());
    r = Interval<Edge>(min, max);
    return true;
}

template <typename Edge>
bool intersect(const Interval<Edge>& lhs, const Interval<Edge>& rhs, Interval<Edge>& r)
{
    if (rhs.max() <= lhs.max() && rhs.max() >= lhs.min())
    {
        Edge min = std::max(lhs.min(), rhs.min());
        Edge max = std::min(lhs.max(), rhs.max());
        r = Interval<Edge>(min, max);
        return true;
    }
    return false;
}