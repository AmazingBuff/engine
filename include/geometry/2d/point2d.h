//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef POINT2D_H
#define POINT2D_H

#include "geometry/type.h"

AMAZING_NAMESPACE_BEGIN

using Point2D = Eigen::Vector2<Float>;
using Vector2D = Eigen::Vector2<Float>;

// define 2d cross, only to get value
inline Float cross(const Vector2D& v1, const Vector2D& v2)
{
    return v1.x() * v2.y() - v2.x() * v1.y();
}


AMAZING_NAMESPACE_END
#endif //POINT2D_H
