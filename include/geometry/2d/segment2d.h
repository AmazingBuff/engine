//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef SEGMENT2D_H
#define SEGMENT2D_H

#include "point2d.h"

AMAZING_NAMESPACE_BEGIN

class Segment2D
{
public:
    Segment2D(const Point2D& p1, const Point2D& p2);

    // unnormalized direction
    NODISCARD Vector2D direction() const;
    NODISCARD Float length() const;
    NODISCARD DirectionDetection detect_point_direction(const Point2D& p) const;
private:
    Point2D m_start;
    Point2D m_end;
};

AMAZING_NAMESPACE_END

#endif //SEGMENT2D_H
