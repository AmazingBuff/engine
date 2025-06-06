//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef RECTANGLE2D_H
#define RECTANGLE2D_H

#include "geometry/2d/point2d.h"

AMAZING_NAMESPACE_BEGIN

class Rectangle2D
{
public:
    Rectangle2D(Float x, Float y, Float width, Float height);
    Rectangle2D(const Point2D& p_min, const Point2D& p_max);

private:
    Point2D m_min;
    Point2D m_max;
};

AMAZING_NAMESPACE_END

#endif //RECTANGLE2D_H
