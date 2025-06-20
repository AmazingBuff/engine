//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Triangle3D final : public Primitive
{
public:
    Triangle3D(const Point3D& p0, const Point3D& p1, const Point3D& p2);
    NODISCARD PrimitiveType type() const override;

    NODISCARD Vector3D normal() const;
    NODISCARD Float d() const;
    NODISCARD Point3D vertex(size_t i) const;
    NODISCARD Float area() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const override;
private:
    Point3D m_vertices[3];
};

AMAZING_NAMESPACE_END
#endif //TRIANGLE3D_H
