//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CAPSULE_H
#define CAPSULE_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Capsule final : public Primitive
{
public:
    Capsule(const Point3D& up_center, const Point3D& down_center, Float radius);
    NODISCARD PrimitiveType type() const override;

    NODISCARD Point3D up_center() const;
    NODISCARD Point3D down_center() const;
    NODISCARD Float radius() const;
    NODISCARD Float height() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const override;
private:
    Point3D m_up_center;
    Point3D m_down_center;
    Float m_radius;
};

AMAZING_NAMESPACE_END
#endif //CAPSULE_H
