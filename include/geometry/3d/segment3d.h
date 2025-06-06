//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef SEGMENT3D_H
#define SEGMENT3D_H

#include "point3d.h"

AMAZING_NAMESPACE_BEGIN

class Segment3D
{
public:
    Segment3D(const Point3D& p1, const Point3D& p2);

    // unnormalized direction
    NODISCARD Vector3D direction() const;
    NODISCARD Point3D origin() const;
    NODISCARD Float length() const;
    NODISCARD DirectionDetection detect_point_direction(const Point3D& p) const;
    NODISCARD Float distance(const Point3D& p) const;
private:
    Point3D m_start;
    Point3D m_end;
};

AMAZING_NAMESPACE_END

#endif //SEGMENT3D_H
