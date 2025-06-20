//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Sphere final : public Primitive
{
public:
    Sphere(const Point3D& center, Float radius);
    NODISCARD PrimitiveType type() const override;

    NODISCARD Point3D center() const;
    NODISCARD Float radius() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const override;
private:
    Point3D m_center;
    Float m_radius;
};


AMAZING_NAMESPACE_END

#endif //SPHERE_H
