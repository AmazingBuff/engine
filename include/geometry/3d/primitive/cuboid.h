//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CUBOID_H
#define CUBOID_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Cuboid
{
public:
    Cuboid(Float x, Float y, Float z, Float width, Float height, Float depth);
    Cuboid(const Point3D& point, Float width, Float height, Float depth);
    Cuboid(const Point3D& p_min, const Point3D& p_max);

    NODISCARD Point3D min() const;
    NODISCARD Point3D max() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const;
private:
    Point3D m_min;
    Point3D m_max;
};

AMAZING_NAMESPACE_END
#endif //CUBOID_H
