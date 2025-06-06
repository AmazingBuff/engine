#include <common.h>


int main()
{
    Segment3D s1(Point3D(0, 0, 0), Point3D(0, 0, 4));
    Segment3D s2(Point3D(0, -2, 0), Point3D(0, 1, 2));

    Point3D intersection;
    if (intersect_segment_segment(s1, s2, intersection))
        std::cout << intersection << std::endl;

    Face3D face(Point3D(0, 0, 1), Vector3D(0, 0, 1));
    if (intersect_segment_face(s2, face, intersection))
        std::cout << intersection << std::endl;

    Triangle3D triangle(Point3D(-1, -1, 1), Vector3D(1, -1, 1), Vector3D(0, 1, 1));
    if (intersect_segment_triangle(s2, triangle, intersection))
        std::cout << intersection << std::endl;

    Cuboid box(Point3D(-1, -1, -1), Vector3D(1, 1, 1));
    if (intersect_segment_cuboid(s2, box, intersection))
        std::cout << intersection << std::endl;

    return 0;
}