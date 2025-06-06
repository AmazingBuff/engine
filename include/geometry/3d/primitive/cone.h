//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef CONE_H
#define CONE_H

#include "geometry/3d/point3d.h"

AMAZING_NAMESPACE_BEGIN

class Cone
{
public:
    Cone(const Point3D& center, Float radius, Float height);

private:
    Point3D m_center;
    Float m_radius;
    Float m_height;
};



AMAZING_NAMESPACE_END

#endif //CONE_H
