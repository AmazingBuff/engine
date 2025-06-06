//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include "geometry/2d/point2d.h"

AMAZING_NAMESPACE_BEGIN

class Triangle2D
{
public:
    Triangle2D(const Point2D& p0, const Point2D& p1, const Point2D& p2);

private:
    Point2D m_vertices[3];
};

AMAZING_NAMESPACE_END
#endif //TRIANGLE2D_H
