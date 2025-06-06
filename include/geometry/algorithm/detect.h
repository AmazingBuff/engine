//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef DETECT_H
#define DETECT_H

#include "geometry/2d/point2d.h"

AMAZING_NAMESPACE_BEGIN

// input points must be sorted in x position, and should greater than 2
// output points will be arranged in a counterclockwise direction
Vector<Point2D> detect_convex_hull(const Vector<Point2D>& points);

AMAZING_NAMESPACE_END
#endif //DETECT_H
