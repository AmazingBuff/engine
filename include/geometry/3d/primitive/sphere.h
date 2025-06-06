//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Sphere
{
public:
    Sphere(const Point3D& center, Float radius);

    NODISCARD Point3D center() const;
    NODISCARD Float radius() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const;
private:
    Point3D m_center;
    Float m_radius;
};


AMAZING_NAMESPACE_END

#endif //SPHERE_H
