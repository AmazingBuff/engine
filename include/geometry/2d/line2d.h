//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef LINE2D_H
#define LINE2D_H

#include "segment2d.h"

AMAZING_NAMESPACE_BEGIN

class Line2D
{
public:
    Line2D(const Point2D& o, const Vector2D& d);
    NODISCARD DirectionDetection detect_point_direction(const Point2D& p) const;
private:
    Point2D m_origin;
    Vector2D m_direction;
};

AMAZING_NAMESPACE_END

#endif //LINE2D_H
