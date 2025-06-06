//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef LINE3D_H
#define LINE3D_H

#include "point3d.h"

AMAZING_NAMESPACE_BEGIN

class Line3D
{
public:
    Line3D(const Point3D& o, const Vector3D& d);

private:
    Point3D m_origin;
    Vector3D m_direction;
};

AMAZING_NAMESPACE_END

#endif //LINE3D_H
