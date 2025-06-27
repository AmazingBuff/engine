

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
    Edge min = std::min(lhs.m_min, rhs.m_min);
    Edge max = std::max(lhs.m_max, rhs.m_max);
    r = Interval<Edge>(min, max);
    return true;
}

template <typename Edge>
bool intersect(const Interval<Edge>& lhs, const Interval<Edge>& rhs, Interval<Edge>& r)
{
    if (rhs.m_max <= lhs.m_max && rhs.m_max >= lhs.m_min)
    {
        Edge min = std::max(lhs.m_min, rhs.m_min);
        Edge max = std::min(lhs.m_max, rhs.m_max);
        r = Interval<Edge>(min, max);
        return true;
    }
    return false;
}