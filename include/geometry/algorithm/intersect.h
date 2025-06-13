//
// Created by AmazingBuff on 2025/6/3.
//

#ifndef INTERSECT_H
#define INTERSECT_H

#include "geometry/3d/segment3d.h"
#include "geometry/3d/face3d.h"
#include "geometry/3d/primitive/disc3d.h"
#include "geometry/3d/primitive/rectangle3d.h"
#include "geometry/3d/primitive/triangle3d.h"
#include "geometry/3d/primitive/cuboid.h"
#include "geometry/3d/primitive/sphere.h"
#include "geometry/3d/primitive/cylinder.h"
#include "geometry/3d/primitive/cone.h"
#include "geometry/3d/primitive/capsule.h"

AMAZING_NAMESPACE_BEGIN

// for all intersection detection, if you want to make any contact as intersection, just
// define CONTACT_AS_INTERSECTION macro, otherwise, contact is considered as tangency

// also, if you want to make any containment as intersection, just
// define CONTAINMENT_AS_INTERSECTION macro, otherwise, containment will not be considered as intersection


// 2d



// 3d
bool intersect_segment_segment_unchecked(const Segment3D& s1, const Segment3D& s2);
bool intersect_segment_segment(const Segment3D& s1, const Segment3D& s2, Point3D& intersection);

bool intersect_segment_face_unchecked(const Segment3D& s, const Face3D& f);
bool intersect_segment_face(const Segment3D& s, const Face3D& f, Point3D& intersection);

bool intersect_segment_rectangle_unchecked(const Segment3D& s, const Rectangle3D& r);
bool intersect_segment_rectangle(const Segment3D& s, const Rectangle3D& r, Point3D& intersection);

bool intersect_segment_triangle_unchecked(const Segment3D& s, const Triangle3D& t);
bool intersect_segment_triangle(const Segment3D& s, const Triangle3D& t, Point3D& intersection);

bool intersect_segment_disc_unchecked(const Segment3D& s, const Disc3D& c);
bool intersect_segment_disc(const Segment3D& s, const Disc3D& c, Point3D& intersection);

bool intersect_segment_cuboid_unchecked(const Segment3D& s, const Cuboid& c);
// only give nearest intersection
bool intersect_segment_cuboid(const Segment3D& s, const Cuboid& c, Point3D& intersection);

bool intersect_segment_sphere_unchecked(const Segment3D& s, const Sphere& sp);
// only give nearest intersection
bool intersect_segment_sphere(const Segment3D& s, const Sphere& sp, Point3D& intersection);

bool intersect_segment_cylinder_unchecked(const Segment3D& s, const Cylinder& c);
// only give nearest intersection
bool intersect_segment_cylinder(const Segment3D& s, const Cylinder& c, Point3D& intersection);

bool intersect_segment_cone_unchecked(const Segment3D& s, const Cone& c);
// only give nearest intersection
bool intersect_segment_cone(const Segment3D& s, const Cone& c, Point3D& intersection);

bool intersect_segment_capsule_unchecked(const Segment3D& s, const Capsule& c);
// only give nearest intersection
bool intersect_segment_capsule(const Segment3D& s, const Capsule& c, Point3D& intersection);

AMAZING_NAMESPACE_END

#endif //INTERSECT_H
