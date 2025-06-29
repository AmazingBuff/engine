//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef DISC3D_H
#define DISC3D_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Disc3D final : public Primitive
{
public:
    Disc3D(const Point3D& center, const Vector3D& normal, Float radius);
    NODISCARD PrimitiveType type() const override;

    NODISCARD Point3D center() const;
    NODISCARD Vector3D normal() const;
    NODISCARD Float radius() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const override;
private:
    Point3D m_center;
    Vector3D m_normal;
    Float m_radius;
};

AMAZING_NAMESPACE_END

#endif //DISC3D_H