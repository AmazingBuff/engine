//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/algorithm/intersect.h"

AMAZING_NAMESPACE_BEGIN


bool intersect_segment_segment_unchecked(const Segment3D& s1, const Segment3D& s2)
{
    Vector3D d1 = s1.direction();
    Vector3D d2 = s2.direction();

    Vector3D c = d1.cross(d2);
    Float square_norm = c.dot(c);
    if (!EQUAL_TO_ZERO(square_norm))
    {
        Vector3D v = s1.origin() - s2.origin();
        if (EQUAL_TO_ZERO(v.dot(c)))
        {
            Float t1 = v.cross(d1).dot(c) / square_norm;
            //Float t2 = v.cross(d2).dot(c) / square_norm;

            if (t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error)
                return true;
        }
    }
#ifdef CONTACT_AS_INTERSECTION
    else
    {
        if (s1.detect_point_direction(s2.origin()) != DirectionDetection::e_outer ||
            s1.detect_point_direction(s2.origin() + s2.direction()) != DirectionDetection::e_outer)
            return true;
    }
#endif

    return false;
}

bool intersect_segment_segment(const Segment3D& s1, const Segment3D& s2, Point3D& intersection)
{
    Vector3D d1 = s1.direction();
    Vector3D d2 = s2.direction();

    Vector3D c = d1.cross(d2);
    Float square_norm = c.dot(c);
    if (square_norm > Max_Allowable_Error || square_norm < -Max_Allowable_Error)
    {
        Point3D o1 = s1.origin();
        Point3D o2 = s2.origin();
        Vector3D v = o2 - o1;
        if (std::abs(v.dot(c)) < Max_Allowable_Error)
        {
            Float t1 = v.cross(d1).dot(c) / square_norm;
            //Float t2 = v.cross(d2).dot(c) / square_norm;

            if (t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error)
            {
                intersection = o1 + t1 * d1;
                return true;
            }
        }
    }
#ifdef CONTACT_AS_INTERSECTION
    else
    {
        if (s1.detect_point_direction(s2.origin()) != DirectionDetection::e_outer ||
            s1.detect_point_direction(s2.origin() + s2.direction()) != DirectionDetection::e_outer)
        {
            // countless intersections
            intersection = {std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()};
            return true;
        }
    }
#endif
    return false;
}

bool intersect_segment_face_unchecked(const Segment3D& s, const Face3D& f)
{
    Vector3D o = s.origin() - Point3D(0, 0, 0);
    Vector3D d = s.direction();
    Vector3D n = f.normal();

    Float v = d.dot(n);
    if (!EQUAL_TO_ZERO(v))
    {
        Float t = (f.d() - o.dot(n)) / v;

        if (t > Max_Allowable_Error && t < 1.0 - Max_Allowable_Error)
            return true;
    }
#ifdef CONTACT_AS_INTERSECTION
    else
    {
        if(f.detect_point_direction(s.origin()) == DirectionDetection::e_coplanar)
            return true;
    }
#endif

    return false;
}

bool intersect_segment_face(const Segment3D& s, const Face3D& f, Point3D& intersection)
{
    Vector3D o = s.origin() - Point3D(0, 0, 0);
    Vector3D d = s.direction();
    Vector3D n = f.normal();

    Float v = d.dot(n);
    if (!EQUAL_TO_ZERO(v))
    {
        Float t = (f.d() - o.dot(n)) / v;

        if (t > Max_Allowable_Error && t < 1.0 - Max_Allowable_Error)
        {
            intersection = o + t * d;
            return true;
        }
    }
#ifdef CONTACT_AS_INTERSECTION
    else
    {
        if(f.detect_point_direction(s.origin()) == DirectionDetection::e_coplanar)
        {
            // countless intersections
            intersection = {std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()};
            return true;
        }
    }
#endif

    return false;
}

bool intersect_segment_rectangle_unchecked(const Segment3D& s, const Rectangle3D& r)
{
    Vector3D n = r.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (r.detect_point_direction(so) == DirectionDetection::e_inner&&
        r.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
        return true; // segment is inside the rectangle
#else
        return false;
#endif

    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Point3D ro = r.origin();
        Vector3D h = r.horizontal();
        Vector3D v = r.vertical();

        Segment3D s1(ro, ro + h);
        Segment3D s2(ro, ro + v);
        Segment3D s3(ro + h, ro + h + v);
        Segment3D s4(ro + v, ro + h + v);

        if (intersect_segment_segment_unchecked(s, s1) || intersect_segment_segment_unchecked(s, s2) ||
            intersect_segment_segment_unchecked(s, s3) || intersect_segment_segment_unchecked(s, s4))
            return true;
    }
    else
    {
        Face3D face(n, r.d());
        Point3D intersection;
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (r.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (r.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }
    return false;
}

bool intersect_segment_rectangle(const Segment3D& s, const Rectangle3D& r, Point3D& intersection)
{
    Vector3D n = r.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (r.detect_point_direction(so) == DirectionDetection::e_inner &&
        r.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
    {
        // countless intersections
        intersection = {std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()};
        return true; // segment is inside the rectangle
    }
#else
        return false;
#endif

    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Point3D ro = r.origin();
        Vector3D h = r.horizontal();
        Vector3D v = r.vertical();

        Segment3D s1(ro, ro + h);
        Segment3D s2(ro, ro + v);
        Segment3D s3(ro + h, ro + h + v);
        Segment3D s4(ro + v, ro + h + v);

        Point3D intersection1, intersection2, intersection3, intersection4;
        Float t = std::numeric_limits<Float>::max(); 

        const static auto func = [&s, &so, &d, &t](const Segment3D& seg, Point3D& intersect)
        {
            if (intersect_segment_segment(s, seg, intersect))
            {
                Vector3D i = intersect - so;
                t = std::min(i.dot(d) / d.dot(d), t);
            }
        };

        func(s1, intersection1);
        func(s2, intersection2);
        func(s3, intersection3);
        func(s4, intersection4);

        if (t != std::numeric_limits<Float>::max())
        {
            intersection = so + t * d;
            return true;
        }
    }
    else
    {
        Face3D face(n, r.d());
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (r.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (r.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }
    return false;
}

bool intersect_segment_triangle_unchecked(const Segment3D& s, const Triangle3D& t)
{
    Vector3D n = t.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (t.detect_point_direction(so) == DirectionDetection::e_inner &&
        t.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
        return true; // segment is inside the rectangle
#else
        return false;
#endif

    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Point3D A = t.vertex(0);
        Point3D B = t.vertex(1);
        Point3D C = t.vertex(2);

        Segment3D s1(A, B);
        Segment3D s2(A, C);
        Segment3D s3(B, C);

        if (intersect_segment_segment_unchecked(s, s1) ||
            intersect_segment_segment_unchecked(s, s2) ||
            intersect_segment_segment_unchecked(s, s3))
            return true;
    }
    else
    {
        Face3D face(n, t.d());
        Point3D intersection;
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (t.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (t.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }

    return false;
}

bool intersect_segment_triangle(const Segment3D& s, const Triangle3D& t, Point3D& intersection)
{
    Vector3D n = t.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (t.detect_point_direction(so) == DirectionDetection::e_inner &&
        t.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
    {
        // countless intersections
        intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
        return true; // segment is inside the rectangle
    }
#else
        return false;
#endif

    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Point3D A = t.vertex(0);
        Point3D B = t.vertex(1);
        Point3D C = t.vertex(2);

        Segment3D s1(A, B);
        Segment3D s2(A, C);
        Segment3D s3(B, C);

        Point3D intersection1, intersection2, intersection3;
        Float t = std::numeric_limits<Float>::max();

        const static auto func = [&s, &so, &d, &t](const Segment3D& seg, Point3D& intersect)
        {
            if (intersect_segment_segment(s, seg, intersect))
            {
                Vector3D i = intersect - so;
                t = std::min(i.dot(d) / d.dot(d), t);
            }
        };

        func(s1, intersection1);
        func(s2, intersection2);
        func(s3, intersection3);

        if (t != std::numeric_limits<Float>::max())
        {
            intersection = so + t * d;
            return true;
        }
    }
    else
    {
        Face3D face(n, t.d());
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (t.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (t.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }
    return false;
}

bool intersect_segment_disc_unchecked(const Segment3D& s, const Disc3D& c)
{
    Vector3D n = c.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (c.detect_point_direction(so) == DirectionDetection::e_inner &&
        c.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
        return true; // segment is inside the rectangle
#else
        return false;
#endif

    Point3D co = c.center();
    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Vector3D v = so - co;

        Float r = c.radius();
        Float A = d.dot(d);
        Float B = 2 * d.dot(v);
        Float C = v.dot(v) - r * r;

        Float discriminant = B * B - 4 * A * C;
        if (discriminant > Max_Allowable_Error)
            return true;
#ifdef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(discriminant))
            return true; // segment is tangent to the circle
#endif
    }
    else
    {
        Face3D face(co, n);
        Point3D intersection;
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (c.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (c.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }

    return false;
}

bool intersect_segment_disc(const Segment3D& s, const Disc3D& c, Point3D& intersection)
{
    Vector3D n = c.normal();
    Vector3D d = s.direction();
    Point3D so = s.origin();

    if (c.detect_point_direction(so) == DirectionDetection::e_inner &&
        c.detect_point_direction(so + d) == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
    {
        // countless intersections
        intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
        return true; // segment is inside the rectangle
    }
#else
        return false;
#endif

    Point3D co = c.center();
    // coplanar
    if (EQUAL_TO_ZERO(n.dot(d)))
    {
        Vector3D v = so - co;

        Float r = c.radius();
        Float A = d.dot(d);
        Float B = 2 * d.dot(v);
        Float C = v.dot(v) - r * r;

        Float discriminant = B * B - 4 * A * C;
        if (discriminant > Max_Allowable_Error)
        {
            Float discriminant_sqrt = std::sqrt(discriminant);
            Float t1 = (-B - discriminant_sqrt) / (2 * A);
            Float t2 = (-B + discriminant_sqrt) / (2 * A);

            if (t1 < -Max_Allowable_Error)
                intersection = so + t2 * d; // t1 is outside the segment
            else if (t2 > 1.0 + Max_Allowable_Error)
                intersection = so + t1 * d; // t2 is outside the segment
            
            return true;
        }
#ifdef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(discriminant))
        {
            Float t = -B / (2 * A);
            intersection = so + t * d;
            return true; // segment is tangent to the circle
        }
#endif
    }
    else
    {
        Face3D face(co, n);
        if (intersect_segment_face(s, face, intersection))
        {
#ifdef CONTACT_AS_INTERSECTION
            if (c.detect_point_direction(intersection) != DirectionDetection::e_outer)
#else
            if (c.detect_point_direction(intersection) == DirectionDetection::e_inner)
#endif
                return true;
        }
    }

    return false;
}

bool intersect_segment_cuboid_unchecked(const Segment3D& s, const Cuboid& c)
{
    Point3D min = c.min();
    Point3D max = c.max();

    Point3D o = s.origin();
    Vector3D d = s.direction();

    // slab
    Float t_min = 0.0, t_max = 1.0;
    if (!EQUAL_TO_ZERO(d.x()))
    {
        Float tx1 = (min.x() - o.x()) / d.x();
        Float tx2 = (max.x() - o.x()) / d.x();

        if (tx1 > tx2)
            swap(tx1, tx2);
        t_min = std::max(t_min, tx1);
        t_max = std::min(t_max, tx2);
    }
    else
    {
        if (o.x() < min.x() - Max_Allowable_Error || o.x() > max.x() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.x() - min.x()) || EQUAL_TO_ZERO(o.x() - max.x()))
            return false;
#endif
    }

    if (!EQUAL_TO_ZERO(d.y()))
    {
        Float ty1 = (min.y() - o.y()) / d.y();
        Float ty2 = (max.y() - o.y()) / d.y();

        if (ty1 > ty2)
            swap(ty1, ty2);
        t_min = std::max(t_min, ty1);
        t_max = std::min(t_max, ty2);
    }
    else
    {
        if (o.y() < min.y() - Max_Allowable_Error || o.y() > max.y() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.y() - min.y()) || EQUAL_TO_ZERO(o.y() - max.y()))
            return false;
#endif
    }

    if (!EQUAL_TO_ZERO(d.z()))
    {
        Float tz1 = (min.z() - o.z()) / d.z();
        Float tz2 = (max.z() - o.z()) / d.z();

        if (tz1 > tz2)
            swap(tz1, tz2);
        t_min = std::max(t_min, tz1);
        t_max = std::min(t_max, tz2);
    }
    else
    {
        if (o.z() < min.z() - Max_Allowable_Error || o.z() > max.z() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.z() - min.z()) || EQUAL_TO_ZERO(o.z() - max.z()))
            return false;
#endif
    }

#ifndef CONTAINMENT_AS_INTERSECTION
    if (EQUAL_TO_ZERO(t_min) && EQUAL_TO_ZERO(t_max - 1.0))
        return false;
#endif

    if (t_min > t_max)
        return false;

    return true;
}

bool intersect_segment_cuboid(const Segment3D& s, const Cuboid& c, Point3D& intersection)
{
    Point3D min = c.min();
    Point3D max = c.max();

    Point3D o = s.origin();
    Vector3D d = s.direction();

    // slab
    Float t_min = 0.0, t_max = 1.0;
    if (!EQUAL_TO_ZERO(d.x()))
    {
        Float tx1 = (min.x() - o.x()) / d.x();
        Float tx2 = (max.x() - o.x()) / d.x();

        if (tx1 > tx2)
            swap(tx1, tx2);
        t_min = std::max(t_min, tx1);
        t_max = std::min(t_max, tx2);
    }
    else
    {
        if (o.x() < min.x() - Max_Allowable_Error || o.x() > max.x() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.x() - min.x()) || EQUAL_TO_ZERO(o.x() - max.x()))
            return false;
#endif
    }

    if (!EQUAL_TO_ZERO(d.y()))
    {
        Float ty1 = (min.y() - o.y()) / d.y();
        Float ty2 = (max.y() - o.y()) / d.y();

        if (ty1 > ty2)
            swap(ty1, ty2);
        t_min = std::max(t_min, ty1);
        t_max = std::min(t_max, ty2);
    }
    else
    {
        if (o.y() < min.y() - Max_Allowable_Error || o.y() > max.y() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.y() - min.y()) || EQUAL_TO_ZERO(o.y() - max.y()))
            return false;
#endif
    }

    if (!EQUAL_TO_ZERO(d.z()))
    {
        Float tz1 = (min.z() - o.z()) / d.z();
        Float tz2 = (max.z() - o.z()) / d.z();

        if (tz1 > tz2)
            swap(tz1, tz2);
        t_min = std::max(t_min, tz1);
        t_max = std::min(t_max, tz2);
    }
    else
    {
        if (o.z() < min.z() - Max_Allowable_Error || o.z() > max.z() + Max_Allowable_Error)
            return false;
#ifndef CONTACT_AS_INTERSECTION
        if (EQUAL_TO_ZERO(o.z() - min.z()) || EQUAL_TO_ZERO(o.z() - max.z()))
            return false;
#endif
    }

    if (t_min > t_max)
        return false;

    if (EQUAL_TO_ZERO(t_min))
    {
        if (EQUAL_TO_ZERO(t_max - 1.0))
#ifndef CONTAINMENT_AS_INTERSECTION
            return false;
#else
            intersection = {std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()};
#endif
        else
            intersection = o + t_max * d;
    }
    else
        intersection = o + t_min * d;

    return true;
}

bool intersect_segment_sphere_unchecked(const Segment3D& s, const Sphere& sp)
{
    // find nearest point
    Point3D so = s.origin();
    Vector3D o = sp.center() - so;
    Vector3D d = s.direction();
    Float t = o.dot(d) / d.dot(d);
    if (t < Max_Allowable_Error)
        t = 0;
    else if (t > 1.0 - Max_Allowable_Error)
        t = 1.0;
    Point3D p = so + t * d;

#ifdef CONTACT_AS_INTERSECTION
    if (sp.detect_point_direction(p) == DirectionDetection::e_border)
        return true;
#endif
    if (sp.detect_point_direction(p) == DirectionDetection::e_inner)
#ifndef CONTAINMENT_AS_INTERSECTION
        if (sp.detect_point_direction(so) == DirectionDetection::e_outer ||
            sp.detect_point_direction(so + d) == DirectionDetection::e_outer)
#endif
            return true;

    return false;
}

bool intersect_segment_sphere(const Segment3D& s, const Sphere& sp, Point3D& intersection)
{
    // find nearest point
    Point3D so = s.origin();
    Vector3D o = sp.center() - so;
    Vector3D d = s.direction();
    Float t = o.dot(d) / d.dot(d);
    if (t < Max_Allowable_Error)
        t = 0;
    else if (t > 1.0 - Max_Allowable_Error)
        t = 1.0;
    Point3D p = so + t * d;

#ifdef CONTACT_AS_INTERSECTION
    if (sp.detect_point_direction(p) == DirectionDetection::e_border)
    {
        intersection = p;
        return true;
    }
#endif
    if (sp.detect_point_direction(p) == DirectionDetection::e_inner)
    {
        DirectionDetection sod = sp.detect_point_direction(so);
        if (sod == DirectionDetection::e_outer)
        {
            Float r = sp.radius();
            Float sop_squared = t * t * d.dot(d);
            Float o_squared = o.dot(o);
            Float l = d.norm();

            Float k = std::sqrt(r * r - o_squared + sop_squared) / l;
            intersection = p - k * d;
        }
        else if (sod == DirectionDetection::e_border)
            intersection = so;
        else
        {
            Vector3D sd = so + d;
            DirectionDetection sdd = sp.detect_point_direction(sd);
            if (sdd == DirectionDetection::e_outer)
            {
                Float r = sp.radius();
                Float sdp_squared = (1.0 - t) * (1.0 - t) * d.dot(d);

                Vector3D od = sp.center() - sd;
                Float od_squared = od.dot(od);
                Float l = d.norm();

                Float k = std::sqrt(r * r - od_squared + sdp_squared) / l;
                intersection = p + k * d;
            }
            else if (sdd == DirectionDetection::e_border)
                intersection = sd;
            else
#ifdef CONTAINMENT_AS_INTERSECTION
                intersection = {std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max()};
#else
                return false;
#endif
        }

        return true;
    }
    return false;
}

bool intersect_segment_cylinder_unchecked(const Segment3D& s, const Cylinder& c)
{
    Point3D so = s.origin();
    Vector3D d = s.direction();

    DirectionDetection sd = c.detect_point_direction(so);
    DirectionDetection dd = c.detect_point_direction(so + d);
    if (sd == DirectionDetection::e_inner)
    {
        if (dd == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
        else if (dd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
    }

    if ((dd == DirectionDetection::e_inner && sd == DirectionDetection::e_border) ||
        (dd == DirectionDetection::e_border && sd == DirectionDetection::e_border))
#ifdef CONTACT_AS_INTERSECTION
        return true;
#else
        return false;
#endif

    Point3D up = c.up_center();
    Point3D down = c.down_center();
    Float r = c.radius();

    Vector3D cc = up - down;
    Vector3D o = so - down;

    Float a = cc.dot(cc);

    // projection test
    Vector3D e = o + d;

    Float i1 = o.dot(cc) / a;
    Float i2 = e.dot(cc) / a;

    if (i1 > i2)
        swap(i1, i2);

    // i2 > i1
    if ((i1 < -Max_Allowable_Error && i2 > Max_Allowable_Error) ||
        (i1 > Max_Allowable_Error && i1 < 1.0 - Max_Allowable_Error))
    {
        if (EQUAL_TO_ZERO((cc.cross(d)).squaredNorm()))
        {
            // cc // d
            Vector3D od = so - down;
            Vector3D od_perp = od - od.dot(cc) / cc.dot(cc) * cc;
            if (od_perp.norm() < r - Max_Allowable_Error)
                return true;
            
            return false;
        }

        Float m = d.dot(cc) / a;
        Float n = o.dot(cc) / a;

        Vector3D d_perp = d - m * cc;
        Vector3D o_perp = o - n * cc;

        // equation, from ||o_perp + t * d_perp|| = r
        Float A = d_perp.dot(d_perp);
        Float B = 2.0 * d_perp.dot(o_perp);
        Float C = o_perp.dot(o_perp) - r * r;
        Float discriminant = B * B - 4 * A * C;
        if (discriminant < -Max_Allowable_Error)
            return false;

        if (EQUAL_TO_ZERO(discriminant))
#ifdef CONTACT_AS_INTERSECTION
        {
            Float t = -B / (2 * A);
            if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
                return false;
            else
                return true;
        }
#else
            return false;
#endif

        Float discriminant_sqrt = std::sqrt(discriminant);
        Float t1 = (-B - discriminant_sqrt) / (2 * A);
        Float t2 = (-B + discriminant_sqrt) / (2 * A);

        if ((t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error) ||
            (t2 > Max_Allowable_Error && t2 < 1.0 - Max_Allowable_Error) ||
            (t1 < -Max_Allowable_Error && t2 > 1.0 + Max_Allowable_Error))
            return true;
    }

    return false;
}

bool intersect_segment_cylinder(const Segment3D& s, const Cylinder& c, Point3D& intersection)
{
    Point3D so = s.origin();
    Vector3D d = s.direction();

    DirectionDetection sd = c.detect_point_direction(so);
    DirectionDetection dd = c.detect_point_direction(so + d);
    if (sd == DirectionDetection::e_inner)
    {
        if (dd == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
        {
            // countless intersections
            intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
            return true;
        }
#else
            return false;
#endif
        else if (dd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
        {
            intersection = so + d; // segment is tangent to the cylinder at the end point of the segment
            return true;
        }
#else
            return false;
#endif
    }

    if (dd == DirectionDetection::e_inner && sd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
    {
        intersection = so; // segment is tangent to the cylinder at the start point of the segment
        return true;
    }
#else
        return false;
#endif

    if (dd == DirectionDetection::e_border && sd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
    {
        // countless intersections
        intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
        return true;
    }
#else
        return false;
#endif

    Point3D up = c.up_center();
    Point3D down = c.down_center();
    Float r = c.radius();

    Vector3D cc = up - down;
    Vector3D o = so - down;

    Float a = cc.dot(cc);

    // projection test
    Vector3D e = o + d;

    Float i1 = o.dot(cc) / a;
    Float i2 = e.dot(cc) / a;

    if (i1 > i2)
        swap(i1, i2);

    // i2 > i1
    if ((i1 < -Max_Allowable_Error && i2 > Max_Allowable_Error) ||
        (i1 > Max_Allowable_Error && i1 < 1.0 - Max_Allowable_Error))
    {
        if (EQUAL_TO_ZERO((cc.cross(d)).squaredNorm()))
        {
            // cc // d
            Vector3D od = so - down;
            Vector3D od_perp = od - od.dot(cc) / cc.dot(cc) * cc;
            if (od_perp.norm() < r - Max_Allowable_Error)
            {
                Vector3D nn = cc.normalized();
                Disc3D c1(down, nn, r);
                Disc3D c2(up, nn, r);

                Point3D intersection1, intersection2;
                Float t1 = std::numeric_limits<Float>::max(), t2 = std::numeric_limits<Float>::max();
                if (intersect_segment_disc(s, c1, intersection1))
                {
                    Vector3D si = intersection1 - so;
                    t1 = si.dot(d) / d.dot(d);
                }

                if (intersect_segment_disc(s, c2, intersection2))
                {
                    Vector3D si = intersection2 - so;
                    t2 = si.dot(d) / d.dot(d);
                }

                if (t1 < t2)
                    swap(t1, t2);

                if (t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error)
                    intersection = so + t1 * d;

                return true;
            }
            return false;
        }

        Float m = d.dot(cc) / a;
        Float n = o.dot(cc) / a;

        Vector3D d_perp = d - m * cc;
        Vector3D o_perp = o - n * cc;

        // equation, from ||o_perp + t * d_perp|| = r
        Float A = d_perp.dot(d_perp);
        Float B = 2.0 * d_perp.dot(o_perp);
        Float C = o_perp.dot(o_perp) - r * r;
        Float discriminant = B * B - 4 * A * C;
        if (discriminant < -Max_Allowable_Error)
            return false;

        if (EQUAL_TO_ZERO(discriminant))
#ifdef CONTACT_AS_INTERSECTION
        {
            Float t = -B / (2 * A);
            if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
                return false;
            else
            {
                intersection = so + t * d;
                return true;        
            }
        }
#else
            return false;
#endif

        Float discriminant_sqrt = std::sqrt(discriminant);
        Float t1 = (-B - discriminant_sqrt) / (2 * A);
        Float t2 = (-B + discriminant_sqrt) / (2 * A);

        // t1 < t2
        if (t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error)
            intersection = so + t1 * d;
        else if (t2 > Max_Allowable_Error && t2 < 1.0 - Max_Allowable_Error)
        {
            Disc3D d1(down, cc, r);
            Disc3D d2(up, cc, r);
            Point3D intersection1, intersection2;
            if (intersect_segment_disc(s, d1, intersection1))
            {
                Float k1 = (intersection1 - so).dot(d) / d.dot(d);
                if (k1 > Max_Allowable_Error && k1 < t2)
                    t2 = k1;
            }
            if (intersect_segment_disc(s, d2, intersection2))
            {
                Float k2 = (intersection2 - so).dot(d) / d.dot(d);
                if (k2 > Max_Allowable_Error && k2 < t2)
                    t2 = k2;
            }
            intersection = so + t2 * d;
        }
        else if (t1 < -Max_Allowable_Error && t2 > 1.0 + Max_Allowable_Error)
        {
            Disc3D d1(down, cc, r);
            Disc3D d2(up, cc, r);
            Point3D intersection1, intersection2;
            Float k = 1.0 - Max_Allowable_Error;
            if (intersect_segment_disc(s, d1, intersection1))
            {
                Float k1 = (intersection1 - so).dot(d) / d.dot(d);
                if (k1 > Max_Allowable_Error && k1 < k)
                    k = k1;
            }
            if (intersect_segment_disc(s, d2, intersection2))
            {
                Float k2 = (intersection2 - so).dot(d) / d.dot(d);
                if (k2 > Max_Allowable_Error && k2 < k)
                    k = k2;
            }
            intersection = so + k * d;
        }
        else
            return false;

        return true;
    }

    return false;
}

bool intersect_segment_cone_unchecked(const Segment3D& s, const Cone& c)
{
    Point3D so = s.origin();
    Vector3D d = s.direction();

    DirectionDetection sd = c.detect_point_direction(so);
    DirectionDetection dd = c.detect_point_direction(so + d);
    if (sd == DirectionDetection::e_inner)
    {
        if (dd == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
        else if (dd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
    }

    if ((dd == DirectionDetection::e_inner && sd == DirectionDetection::e_border) ||
        (dd == DirectionDetection::e_border && sd == DirectionDetection::e_border))
#ifdef CONTACT_AS_INTERSECTION
        return true;
#else
        return false;
#endif

    Point3D peak = c.peak();
    Point3D center = c.center();
    Float r = c.radius();

    Vector3D cc = peak - center;
    Vector3D o = so - center;

    Float a = cc.dot(cc);

    // projection test
    Vector3D e = o + d;

    Float i1 = o.dot(cc) / a;
    Float i2 = e.dot(cc) / a;

    if (i1 > i2)
        swap(i1, i2);

    // i2 > i1
    if ((i1 < -Max_Allowable_Error && i2 > Max_Allowable_Error) ||
        (i1 > Max_Allowable_Error && i1 < 1.0 - Max_Allowable_Error))
    {
        if (EQUAL_TO_ZERO((cc.cross(d)).squaredNorm()))
        {
            // cc // d
            static auto check_point = [=](const Vector3D& v)
            {
                Vector3D oc = v - center;
                Float t = oc.dot(cc) / a;
                Float radius = std::clamp(1.0 - t, 0.0, 1.0) * r;
                Vector3D oc_perp = oc - t * d;
                if (oc_perp.norm() < radius - Max_Allowable_Error)
                    return true;
                return false;
            };

            if (check_point(so) || check_point(so + d))
                return true;

            return false;
        }

        Float m = d.dot(cc) / a;
        Float n = o.dot(cc) / a;
        Float one_minus_n = 1.0 - n;

        Vector3D d_perp = d - m * cc;
        Vector3D o_perp = o - n * cc;

        // equation, from ||o_perp + t * d_perp|| = k * r
        // where k = 1 - n - m * t
        Float m_r = m * r;
        Float one_minus_n_r = one_minus_n * r;
        Float A = d_perp.dot(d_perp) - m_r * m_r;
        Float B = 2 * (d_perp.dot(o_perp) + m_r * one_minus_n_r);
        Float C = o_perp.dot(o_perp) - one_minus_n_r * one_minus_n_r;
        Float discriminant = B * B - 4 * A * C;
        if (discriminant < -Max_Allowable_Error)
            return false;

        if (EQUAL_TO_ZERO(discriminant))
#ifdef CONTACT_AS_INTERSECTION
        {
            Float t = -B / (2 * A);
            if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
                return false;
            else
                return true;
        }
#else
            return false;
#endif

        Float discriminant_sqrt = std::sqrt(discriminant);
        Float t1 = (-B - discriminant_sqrt) / (2 * A);
        Float t2 = (-B + discriminant_sqrt) / (2 * A);

        if ((t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error) ||
            (t2 > Max_Allowable_Error && t2 < 1.0 - Max_Allowable_Error) ||
            (t1 < -Max_Allowable_Error && t2 > 1.0 + Max_Allowable_Error))
            return true;
    }

    return false;
}
// only give nearest intersection
bool intersect_segment_cone(const Segment3D& s, const Cone& c, Point3D& intersection)
{
    Point3D so = s.origin();
    Vector3D d = s.direction();

    DirectionDetection sd = c.detect_point_direction(so);
    DirectionDetection dd = c.detect_point_direction(so + d);
    if (sd == DirectionDetection::e_inner)
    {
        if (dd == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
        {
            // countless intersections
            intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
            return true;
        }
#else
            return false;
#endif
        else if (dd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
        {
            intersection = so + d; // segment is tangent to the cylinder at the end point of the segment
            return true;
        }
#else
            return false;
#endif
    }

    if (dd == DirectionDetection::e_inner && sd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
    {
        intersection = so; // segment is tangent to the cylinder at the start point of the segment
        return true;
    }
#else
        return false;
#endif

    if (dd == DirectionDetection::e_border && sd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
    {
        // countless intersections
        intersection = { std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max(), std::numeric_limits<Float>::max() };
        return true;
    }
#else
        return false;
#endif

    Point3D peak = c.peak();
    Point3D center = c.center();
    Float r = c.radius();

    Vector3D cc = peak - center;
    Vector3D o = so - center;

    Float a = cc.dot(cc);

    // projection test
    Vector3D e = o + d;

    Float i1 = o.dot(cc) / a;
    Float i2 = e.dot(cc) / a;

    if (i1 > i2)
        swap(i1, i2);

    // i2 > i1
    if ((i1 < -Max_Allowable_Error && i2 > Max_Allowable_Error) ||
        (i1 > Max_Allowable_Error && i1 < 1.0 - Max_Allowable_Error))
    {
        if (EQUAL_TO_ZERO((cc.cross(d)).squaredNorm()))
        {
            // cc // d
            static auto check_point = [=](const Vector3D& v, Vector3D& perp)
            {
                Vector3D oc = v - center;
                Float t = oc.dot(cc) / a;
                Float radius = std::clamp(1.0 - t, 0.0, 1.0) * r;
                Vector3D oc_perp = oc - t * d;
                if (oc_perp.norm() < radius - Max_Allowable_Error)
                {
                    perp = oc_perp;
                    return true;
                }
                return false;
            };

            Vector3D perp;
            bool p1 = check_point(so, perp);
            bool p2 = check_point(so + d, perp);

            if (p1 && p2)
            {
                // one inner point, one outer point through disc
                Disc3D disc(center, cc, r);
                if (intersect_segment_disc(s, disc, intersection))
                    return true;
            }
            else if (p1 || p2)
            {
                // one inner point, one outer point through side
                Float radius = perp.norm();
                Float h = radius * c.height() / r;
                Float t = 1.0 - h / cc.norm();
                intersection = center + t * cc + perp;
                return true;
            }

            return false;
        }

        Float m = d.dot(cc) / a;
        Float n = o.dot(cc) / a;
        Float one_minus_n = 1.0 - n;

        Vector3D d_perp = d - m * cc;
        Vector3D o_perp = o - n * cc;

        // equation, from ||o_perp + t * d_perp|| = k * r
        // where k = 1 - n - m * t
        Float m_r = m * r;
        Float one_minus_n_r = one_minus_n * r;
        Float A = d_perp.dot(d_perp) - m_r * m_r;
        Float B = 2 * (d_perp.dot(o_perp) + m_r * one_minus_n_r);
        Float C = o_perp.dot(o_perp) - one_minus_n_r * one_minus_n_r;
        Float discriminant = B * B - 4 * A * C;
        if (discriminant < -Max_Allowable_Error)
            return false;

        if (EQUAL_TO_ZERO(discriminant))
#ifdef CONTACT_AS_INTERSECTION
        {
            Float t = -B / (2 * A);
            if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
                return false;
            else
            {
                intersection = so + t * d;
                return true;
            }
        }
#else
            return false;
#endif

        Float discriminant_sqrt = std::sqrt(discriminant);
        Float t1 = (-B - discriminant_sqrt) / (2 * A);
        Float t2 = (-B + discriminant_sqrt) / (2 * A);

        // t1 < t2
        if (t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error)
            intersection = so + t1 * d;
        else if (t2 > Max_Allowable_Error && t2 < 1.0 - Max_Allowable_Error)
        {
            Disc3D disc(center, cc, r);
            Point3D intersection1;
            if (intersect_segment_disc(s, disc, intersection1))
            {
                Float k1 = (intersection1 - so).dot(d) / d.dot(d);
                if (k1 > Max_Allowable_Error && k1 < t2)
                    t2 = k1;
            }
            intersection = so + t2 * d;
        }
        else if (t1 < -Max_Allowable_Error && t2 > 1.0 + Max_Allowable_Error)
        {
            Disc3D disc(center, cc, r);
            Point3D intersection1;
            Float k = 1.0 - Max_Allowable_Error;
            if (intersect_segment_disc(s, disc, intersection1))
            {
                Float k1 = (intersection1 - so).dot(d) / d.dot(d);
                if (k1 > Max_Allowable_Error && k1 < k)
                    k = k1;
            }
            intersection = so + k * d;
        }
        else
            return false;

        return true;
    }

    return false;
}

// todo: check capsule
bool intersect_segment_capsule_unchecked(const Segment3D& s, const Capsule& c)
{
    Point3D so = s.origin();
    Vector3D d = s.direction();

    DirectionDetection sd = c.detect_point_direction(so);
    DirectionDetection dd = c.detect_point_direction(so + d);
    if (sd == DirectionDetection::e_inner)
    {
        if (dd == DirectionDetection::e_inner)
#ifdef CONTAINMENT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
        else if (dd == DirectionDetection::e_border)
#ifdef CONTACT_AS_INTERSECTION
            return true;
#else
            return false;
#endif
    }

    if ((dd == DirectionDetection::e_inner && sd == DirectionDetection::e_border) ||
        (dd == DirectionDetection::e_border && sd == DirectionDetection::e_border))
#ifdef CONTACT_AS_INTERSECTION
        return true;
#else
        return false;
#endif

    Point3D up = c.up_center();
    Point3D down = c.down_center();
    Float r = c.radius();

    Vector3D cc = up - down;
    Vector3D o = so - down;

    Float a = cc.dot(cc);

    // projection test
    Vector3D e = o + d;

    Float i1 = o.dot(cc) / a;
    Float i2 = e.dot(cc) / a;

    if (i1 > i2)
        swap(i1, i2);

    // i2 > i1
    if ((i1 < -Max_Allowable_Error && i2 > Max_Allowable_Error) ||
        (i1 > Max_Allowable_Error && i1 < 1.0 - Max_Allowable_Error))
    {
        if (EQUAL_TO_ZERO((cc.cross(d)).squaredNorm()))
        {
            // cc // d
            Vector3D od = so - down;
            Vector3D od_perp = od - od.dot(cc) / cc.dot(cc) * cc;
            if (od_perp.norm() < r - Max_Allowable_Error)
                return true;

            return false;
        }

        Float m = d.dot(cc) / a;
        Float n = o.dot(cc) / a;

        Vector3D d_perp = d - m * cc;
        Vector3D o_perp = o - n * cc;

        // equation, from ||o_perp + t * d_perp|| = r
        Float A = d_perp.dot(d_perp);
        Float B = 2.0 * d_perp.dot(o_perp);
        Float C = o_perp.dot(o_perp) - r * r;
        Float discriminant = B * B - 4 * A * C;
        if (discriminant < -Max_Allowable_Error)
            return false;

        if (EQUAL_TO_ZERO(discriminant))
#ifdef CONTACT_AS_INTERSECTION
        {
            Float t = -B / (2 * A);
            if (t < -Max_Allowable_Error || t > 1.0 + Max_Allowable_Error)
                return false;
            else
                return true;
        }
#else
            return false;
#endif

        Float discriminant_sqrt = std::sqrt(discriminant);
        Float t1 = (-B - discriminant_sqrt) / (2 * A);
        Float t2 = (-B + discriminant_sqrt) / (2 * A);

        if ((t1 > Max_Allowable_Error && t1 < 1.0 - Max_Allowable_Error) ||
            (t2 > Max_Allowable_Error && t2 < 1.0 - Max_Allowable_Error) ||
            (t1 < -Max_Allowable_Error && t2 > 1.0 + Max_Allowable_Error))
            return true;
    }

    return false;
}
// only give nearest intersection
bool intersect_segment_capsule(const Segment3D& s, const Capsule& c, Point3D& intersection)
{

    return false;
}


AMAZING_NAMESPACE_END