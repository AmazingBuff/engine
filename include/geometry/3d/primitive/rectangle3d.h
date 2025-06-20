//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef RECTANGLE3D_H
#define RECTANGLE3D_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Rectangle3D final : public Primitive
{
public:
    Rectangle3D(const Point3D& o, const Point3D& h, const Point3D& v);
    //Rectangle3D(const Point3D& o, const Vector3D& u, const Vector3D& v);

    NODISCARD PrimitiveType type() const override;

    NODISCARD Vector3D normal() const;
    NODISCARD Float d() const;
    NODISCARD Point3D origin() const;
    NODISCARD Vector3D horizontal() const;
    NODISCARD Vector3D vertical() const;
    NODISCARD Float area() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const override;
private:
    Point3D m_origin;
    Point3D m_horizontal;
    Point3D m_vertical;
};


AMAZING_NAMESPACE_END

#endif //RECTANGLE3D_H
