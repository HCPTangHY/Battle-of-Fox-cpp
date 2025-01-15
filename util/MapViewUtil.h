#pragma once

#include "Container.h"

namespace BoF {

double get_sq_dist(const Array& p1, const Array& p2) {
    double x1 = nlohmann::json(p1)[0];
    double y1 = nlohmann::json(p1)[1];
    double x2 = nlohmann::json(p2)[0];
    double y2 = nlohmann::json(p2)[1];
    double dx = x1 - x2;
    double dy = y1 - y2;
    return dx * dx + dy * dy;
}

double get_sq_seg_dist(const Array& p, const Array& p1, const Array& p2) {
    double x1 = nlohmann::json(p)[0];
    double y1 = nlohmann::json(p)[1];
    double x = nlohmann::json(p1)[0];
    double y = nlohmann::json(p1)[1];
    double x2 = nlohmann::json(p2)[0];
    double y2 = nlohmann::json(p2)[1];
    double dx = x2 - x;
    double dy = y2 - y;

    if (dx != 0 || dy != 0) {
        double t = ((x - x1) * dx + (y - y1) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = x2;
            y = y2;
        } else if (t > 0) {
            x += dx * t;
            y += dy * t;
        }
    }
    dx = x1 - x;
    dy = y1 - y;
    return dx * dx + dy * dy;
}

Array simplify_radial_dist(const Array& points, double sqTolerance) {
    Array prevPoint = points[0];
    Array newPoints = Array({prevPoint});
    Array point;

    for (size_t i = 0; i < points.size(); i++) {
        point = points[i];
        if(!nlohmann::json(point).is_null()) continue;

        if (get_sq_dist(point, prevPoint) > sqTolerance) {
            newPoints.push_back(point);
            prevPoint = point;
        }
    }

    if (prevPoint!= point) {
        newPoints.push_back(point);
    }
    return newPoints;
}

void simplify_DP_step(const Array& points, int32_t first, int32_t last, double sqTolerance, Array& simplified) {
    double maxSqDist = sqTolerance;
    int32_t index = first;

    for (int32_t i = first + 1; i < last; i++) {
        const double sqDist = get_sq_seg_dist(points[i], points[first], points[last]);

        if (sqDist > maxSqDist) {
            index = i;
            maxSqDist = sqDist;
        }
    }
    if (maxSqDist > sqTolerance) {
        if (index - first > 1) simplify_DP_step(points, first, index, sqTolerance, simplified);
        simplified.push_back(points[index]);
        if (last - index > 1) simplify_DP_step(points, index, last, sqTolerance, simplified);
    }

}

Array simplify_douglas_peucker(const Array& points, double sqTolerance) {
    int32_t last = points.size() - 1;

    Array simplified = Array({points[0]});
    simplify_DP_step(points,0, last, sqTolerance,  simplified);
    simplified.push_back(points[last]);

    return simplified;
}

Array simplify_points(const Array& points, double tolerance, bool hightestQuality = false) {
    if (points.size() < 2) return points;

    double sqTolerance = tolerance * tolerance;

    Array rdpoints = hightestQuality? points : simplify_radial_dist(points, tolerance / 2);
    Array dppoints = simplify_douglas_peucker(rdpoints, sqTolerance);

    return dppoints;
}

}