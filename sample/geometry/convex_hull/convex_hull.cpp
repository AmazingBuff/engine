#include <common.h>

int main()
{
    Vector<Point2D> points = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0.5, 0.5 } };

    // sort before convex hull detect
    sort(points.begin(), points.end(), [](const Point2D& a, const Point2D& b)
    {
        if (a.x() < b.x() || (a.x() == b.x() && a.y() < b.y()))
            return true;
        return false;
    });

    Vector<Point2D> convex_hull = detect_convex_hull(points);

    for (size_t i = 0; i < convex_hull.size(); i++)
    {
        std::cout << convex_hull[i] << std::endl;
    }

    return 0;
}