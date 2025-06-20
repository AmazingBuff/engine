//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CUBOID_H
#define CUBOID_H

#include "primitive.h"

AMAZING_NAMESPACE_BEGIN

class Cuboid final : public Primitive
{
public:
    Cuboid(Float x, Float y, Float z, Float width, Float height, Float depth);
    Cuboid(const Point3D& point, Float width, Float height, Float depth);
    Cuboid(const Point3D& p_min, const Point3D& p_max);
    NODISCARD PrimitiveType type() const override;


    NODISCARD Point3D min() const;
    NODISCARD Point3D max() const;
    NODISCARD AABB aabb() const override;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& point) const override;
private:
    Point3D m_min;
    Point3D m_max;
};

AMAZING_NAMESPACE_END
#endif //CUBOID_H
