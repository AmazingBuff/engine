//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef DISC2D_H
#define DISC2D_H

#include "geometry/2d/point2d.h"

AMAZING_NAMESPACE_BEGIN

class Disc2D
{
public:
    Disc2D(const Point2D& center, Float radius);

private:
    Point2D m_center;
    Float m_radius;
};

AMAZING_NAMESPACE_END

#endif //DISC2D_H
