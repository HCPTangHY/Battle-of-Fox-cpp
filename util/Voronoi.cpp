#include "Voronoi.h"

namespace BoF {
Voronoi::Voronoi(std::shared_ptr<delaunator::Delaunator> delaunay, Array points, int32_t pointsN) {
    this->delaunay = delaunay;
    this->points = points;
    this->pointsN = pointsN;

    this->cells = Dict();
    this->cells.array("v",pointsN); // cells vertices 单元顶点
    this->cells.array("c",pointsN); // adjacent cells 相邻单元
    this->cells.array("b",pointsN); // near-bor 边界单元
    this->vertices = Dict();
    this->vertices.array("p"); // vertices coordinates 顶点坐标
    this->vertices.array("v"); // neighboring vertices 相邻顶点
    this->vertices.array("c"); // adjacent cells 相邻单元

    Array cellsV = this->cells["v"];
    Array cellsC = this->cells["c"]; 
    for (size_t e = 0; e < this->delaunay->triangles.size(); e++) {
        int32_t p = this->delaunay->triangles[this->next_halfedge(e)];
        if (p < this->pointsN && (p >= cellsC.size() || !cellsC.has(p))) {
            Array edges = this->edges_around_point(e);

            auto p_arr_v = Array();
            for (size_t i = 0; i < edges.size(); i++) {
                int32_t triangle = this->triangle_of_edge(edges[i]);
                p_arr_v.push(triangle);
            }

            auto p_arr_c = Array();
            for (size_t i = 0; i < edges.size(); i++) {
                int index = edges[i];
                auto c = this->delaunay->triangles[index];
                if (c < this->pointsN) {
                    p_arr_c.push(c);
                }
            }

            this->cells["v"][p] = p_arr_v;
            this->cells["c"][p] = p_arr_c;
            this->cells["b"][p] = edges.size() > p_arr_c.size() ? 1 : 0;
        }

        int32_t t = this->triangle_of_edge(e);
        if (!this->vertices.get<Array>("p").has(t)) {
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
        result.push(incoming);
        int32_t outgoing = this->next_halfedge(incoming);
        incoming = this->delaunay->halfedges[outgoing];
    } while (incoming != -1 && incoming != start && result.size() < 20);
    return result;
}

Array Voronoi::triangle_center(int32_t t) {
    Array pot = this->point_of_triangle(t);
    Array vertices;
    for (size_t p = 0; p < pot.size(); p++) {
        vertices.push(this->points[pot.value<size_t>(p)]);
    }
    return this->circumcenter(
        vertices[0],
        vertices[1],
        vertices[2]
    );
}

Array Voronoi::point_of_triangle(int32_t t) {
    Array a = this->edges_of_triangle(t);
    Array result = Array();
    for (size_t i = 0; i < a.size(); i++) {
        int idx = a[i];
        result.push(this->delaunay->triangles[idx]);
    }
    return result;
}

Array Voronoi::circumcenter(Array a, Array b, Array c) {
    double ax = a[0];
    double ay = a[1];
    double bx = b[0];
    double by = b[1];
    double cx = c[0];
    double cy = c[1];
    double ad = ax*ax + ay*ay;
    double bd = bx*bx + by*by;
    double cd = cx*cx + cy*cy;
    double D = 2*(ax*(by - cy) + bx*(cy - ay) + cx*(ay - by));
    return Array::make(
        Value(1 / D * (ad * (by - cy) + bd * (cy - ay) + cd * (ay - by))),
        Value(1 / D * (ad * (cx - bx) + bd * (ax - cx) + cd * (bx - ax)))
    );
}

Array Voronoi::triangles_adjacent_to_triangle(int32_t t) {
    Array triangles = Array();
    Array edges = this->edges_of_triangle(t);
    for (size_t i = 0; i < edges.size(); i++) {
        int idx = edges[i];
        double opposite = this->delaunay->halfedges[idx];
        triangles.push(this->triangle_of_edge(opposite));
    }
    return triangles;
}

Array Voronoi::points_of_triangles(int32_t t) {
    Array edges = this->edges_of_triangle(t);
    Array result = Array();
    for (size_t i = 0; i < edges.size(); i++) {
        result.push(static_cast<int>(this->delaunay->triangles[edges.get<int>(i)]));
    }
    return result;
}
}