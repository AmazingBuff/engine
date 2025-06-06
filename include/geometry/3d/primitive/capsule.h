//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CAPSULE_H
#define CAPSULE_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Capsule
{
public:
    Capsule(const Point3D& center, Float radius, Float height);

private:
    Point3D m_center;
    Float m_radius;
    Float m_height;
};

AMAZING_NAMESPACE_END
#endif //CAPSULE_H
