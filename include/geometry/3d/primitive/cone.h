//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CONE_H
#define CONE_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Cuboid;

class Cone
{
public:
    Cone(const Point3D& center, const Point3D& peak, Float radius);

    NODISCARD Point3D center() const;
    NODISCARD Point3D peak() const;
    NODISCARD Float radius() const;
    NODISCARD Float height() const;
    NODISCARD Cuboid aabb() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const;
private:
    Point3D m_center;
    Point3D m_peak;
    Float m_radius;
};



AMAZING_NAMESPACE_END

#endif //CONE_H
