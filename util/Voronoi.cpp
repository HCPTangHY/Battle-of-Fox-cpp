#include "Voronoi.h"

namespace BoF {
Voronoi::Voronoi(std::shared_ptr<delaunator::Delaunator> delaunay, Array points, int32_t pointsN) {
    this->delaunay = delaunay;
    this->points = points;
    this->pointsN = pointsN;

    this->cells = {
        {"v", Array(pointsN)}, // cells vertices 单元顶点
        {"c", Array(pointsN)}, // adjacent cells 相邻单元
        {"b", Array(pointsN)}, // near-bor 边界单元
    };
    this->vertices = {
        {"p", Array()}, // vertices coordinates 顶点坐标
        {"v", Array()}, // neighboring vertices 相邻顶点
        {"c", Array()}, // adjacent cells 相邻单元
    };

    Array cellsV = this->cells["v"];
    Array cellsC = this->cells["c"]; 
    for (size_t e = 0; e < this->delaunay->triangles.size(); e++) {
        int32_t p = this->delaunay->triangles[this->next_halfedge(e)];
        if (p < this->pointsN && (p >= cellsC.size() || !cellsC[p].is_null())) {
            Array edges = this->edges_around_point(e);

            auto p_arr_v = Array();
            for (size_t i = 0; i < edges.size(); i++) {
                int32_t triangle = this->triangle_of_edge(edges[i]);
                p_arr_v.push_back(triangle);
            }

            auto p_arr_c = Array();
            for (size_t i = 0; i < edges.size(); i++) {
                int index = edges[i];
                auto c = this->delaunay->triangles[index];
                if (c < this->pointsN) {
                    p_arr_c.push_back(c);
                }
            }

            this->cells["v"][p] = p_arr_v;
            this->cells["c"][p] = p_arr_c;
            this->cells["b"][p] = edges.size() > p_arr_c.size() ? 1 : 0;
        }

        int32_t t = this->triangle_of_edge(e);
        if (!this->vertices["p"][t].is_null()) {
            this->vertices["p"][t] = this->triangle_center(t);
            this->vertices["v"][t] = this->triangles_adjacent_to_triangle(t);
            this->vertices["c"][t] = this->points_of_triangles(t);
        }
    }
};

Array Voronoi::edges_around_point(int32_t start) {
    Array result = Array();
    int32_t incoming = start;
    do {
        result.push_back(incoming);
        int32_t outgoing = this->next_halfedge(incoming);
        incoming = this->delaunay->halfedges[outgoing];
    } while (incoming != -1 && incoming != start);
    return result;
}

Array Voronoi::triangle_center(int32_t t) {
    Array points = this->point_of_triangle(t);
    Array vertices;
    for (size_t pi = 0; pi < points.size(); pi++) {
        vertices.push_back(this->points[points[pi]]);
    }
    return this->circumcenter(
        vertices[0],
        vertices[1],
        vertices[2]
    );
}

Array Voronoi::point_of_triangle(int32_t t) {
    Array edges = this->edges_of_triangle(t);
    Array result = Array();
    for (size_t i = 0; i < edges.size(); i++) {
        result.push_back(this->delaunay->triangles[edges[i]]);
    }
    return result;
}

Array Voronoi::circumcenter(Array a, Array b, Array c) {
    double ax = a[0], ay = a[1];
    double bx = b[0], by = b[1]; 
    double cx = c[0], cy = c[1];
    
    // 计算三角形的边长
    double ab = std::hypot(ax-bx, ay-by);
    double bc = std::hypot(bx-cx, by-cy);
    double ca = std::hypot(cx-ax, cy-ay);
    
    // 如果三角形太扁,使用重心代替外心
    double minLen = std::min({ab, bc, ca});
    double maxLen = std::max({ab, bc, ca});
    if(minLen/maxLen < 0.01) {
        return Array({
            (ax + bx + cx)/3.0,
            (ay + by + cy)/3.0
        });
    }

    double D = 2*(ax*(by - cy) + bx*(cy - ay) + cx*(ay - by));
    if(std::abs(D) < 1e-10) {
        return Array({
            (ax + bx + cx)/3.0,
            (ay + by + cy)/3.0
        });
    }

    double ad = ax*ax + ay*ay;
    double bd = bx*bx + by*by;
    double cd = cx*cx + cy*cy;

    double x = (ad*(by - cy) + bd*(cy - ay) + cd*(ay - by))/D;
    double y = (ad*(cx - bx) + bd*(ax - cx) + cd*(bx - ax))/D;
    
    // 限制外心在输入点的范围内
    double minX = std::min({ax, bx, cx});
    double maxX = std::max({ax, bx, cx});
    double minY = std::min({ay, by, cy});
    double maxY = std::max({ay, by, cy});
    
    // 允许一定的外扩,但不能太远
    double margin = std::max(maxX - minX, maxY - minY) * 0.5;
    x = std::clamp(x, minX - margin, maxX + margin);
    y = std::clamp(y, minY - margin, maxY + margin);

    return Array({x, y});
}

Array Voronoi::triangles_adjacent_to_triangle(int32_t t) {
    Array triangles = Array();
    Array edges = this->edges_of_triangle(t);
    for (size_t i = 0; i < edges.size(); i++) {
        int idx = edges[i];
        double opposite = this->delaunay->halfedges[idx];
        triangles.push_back(this->triangle_of_edge(opposite));
    }
    return triangles;
}

Array Voronoi::points_of_triangles(int32_t t) {
    Array edges = this->edges_of_triangle(t);
    Array result = Array();
    for (size_t i = 0; i < edges.size(); i++) {
        result.push_back(static_cast<int>(this->delaunay->triangles[edges[i]]));
    }
    return result;
}
}