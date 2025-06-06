//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H
#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Triangle3D
{
public:
    Triangle3D(const Point3D& p0, const Point3D& p1, const Point3D& p2);

    NODISCARD Vector3D normal() const;
    NODISCARD Float d() const;
    NODISCARD Point3D vertex(size_t i) const;
    NODISCARD Float area() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const;
private:
    Point3D m_vertices[3];
};

AMAZING_NAMESPACE_END
#endif //TRIANGLE3D_H
