//
// Created by AmazingBuff on 2025/6/19.
//

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

enum class PrimitiveType : uint8_t
{
    e_triangle,
    e_rectangle,
    e_disc,
    e_sphere,
    e_cuboid,
    e_cylinder,
    e_cone,
    e_capsule,
    e_mesh,
};

struct AABB
{
    Point3D min;
    Point3D max;
};

class Primitive
{
public:
    virtual ~Primitive() = default;
    NODISCARD virtual PrimitiveType type() const = 0;
    NODISCARD virtual AABB aabb() const = 0;
    NODISCARD virtual DirectionDetection detect_point_direction(const Point3D& p) const = 0;
};

AMAZING_NAMESPACE_END
#endif //PRIMITIVE_H
