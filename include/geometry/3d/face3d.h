//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef FACE3D_H
#define FACE3D_H

#include "point3d.h"

AMAZING_NAMESPACE_BEGIN

class Face3D
{
public:
    // Ax + By + Cz = D
    Face3D(Float A, Float B, Float C, Float D);
    Face3D(const Vector3D& normal, Float D);
    Face3D(const Point3D& p, const Vector3D& normal);

    NODISCARD Vector3D normal() const;
    NODISCARD Float d() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const;
private:
    Vector3D m_normal;
    Float m_d;
};


AMAZING_NAMESPACE_END

#endif //FACE3D_H
