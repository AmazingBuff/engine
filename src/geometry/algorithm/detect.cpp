//
// Created by AmazingBuff on 2025/6/3.
//

#include "geometry/algorithm/detect.h"
#include "geometry/2d/segment2d.h"

AMAZING_NAMESPACE_BEGIN

// eliminate convex hull vertex from last point
static size_t Andrew(const Vector<Point2D>& convex_hull, const Point2D& point, size_t last_index)
{
    do
    {
        const Point2D& p1 = convex_hull[last_index - 2];
        const Point2D& p2 = convex_hull[last_index - 1];
        Segment2D segment(p1, p2);
        if (segment.detect_point_direction(point) == DirectionDetection::e_left)
            break;

        last_index--;
    } while (last_index > 1);

    return last_index;
}

Vector<Point2D> detect_convex_hull(const Vector<Point2D>& points)
{
    size_t size = points.size();
    // Andrew algorithm
    Vector<Point2D> lower_convex_hull(size);
    lower_convex_hull[0] = points[0];
    lower_convex_hull[1] = points[1];

    size_t lower_convex_hull_index = 2;
    for (size_t i = 2; i < size; i++)
    {
        lower_convex_hull_index = Andrew(lower_convex_hull, points[i], lower_convex_hull_index);
        lower_convex_hull[lower_convex_hull_index] = points[i];
        lower_convex_hull_index++;
    }

    Vector<Point2D> upper_convex_hull(size);
    upper_convex_hull[0] = points[size - 1];
    upper_convex_hull[1] = points[size - 2];

    size_t upper_convex_hull_index = 2;
    for (size_t i = size - 2; i > 0; i--)
    {
        upper_convex_hull_index = Andrew(upper_convex_hull, points[i - 1], upper_convex_hull_index);
        upper_convex_hull[upper_convex_hull_index] = points[i - 1];
        upper_convex_hull_index++;
    }

    for (size_t i = 1; i < upper_convex_hull_index - 1; i++)
    {
        lower_convex_hull[lower_convex_hull_index] = upper_convex_hull[i];
        lower_convex_hull_index++;
    }

    lower_convex_hull.resize(lower_convex_hull_index);

    return lower_convex_hull;
}



AMAZING_NAMESPACE_END