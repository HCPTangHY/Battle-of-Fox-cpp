#ifndef VORONOI_H
#define VORONOI_H
#pragma once
#include "Container.h"
#include "../include/delaunator.hpp"

namespace BoF {
class Voronoi {
    /**
     * @brief 创建Voronoi图
     * 
     * 从给定的Delaunator对象、点列表和点数量创建Voronoi图。
     * Voronoi图的构建（可能）使用了Bowyer-Watson算法。
     * Delaunator库使用半边数据结构(DCEL)来表示点和三角形之间的关系。
     * 
     * @param delaunay Delaunator实例，包含triangles和halfedges数组
     * @param points 坐标点列表
     * @param pointsN 点的数量
     */

    public:
        std::shared_ptr<delaunator::Delaunator> delaunay;
        Array points;
        int32_t pointsN;
        Dict cells;
        Dict vertices;
        Voronoi(std::shared_ptr<delaunator::Delaunator> delaunay, Array points, int32_t pointsN);

        Array edges_around_point(int32_t start);
        int32_t triangle_of_edge(int32_t e) {return floor(e/3);}
        int32_t next_halfedge(int32_t e) {return (e%3 == 2)? e-2 : e+1;}
        int32_t prev_halfedge(int32_t e) {return (e%3 == 0)? e+2 : e-1;}
        Array edges_of_triangle(int32_t t) {return Array({(t*3), (t*3+1), (t*3+2)});}
        Array triangle_center(int32_t t);
        Array point_of_triangle(int32_t t);
        Array circumcenter(Array a, Array b, Array c);
        Array triangles_adjacent_to_triangle(int32_t t);
        Array points_of_triangles(int32_t t);
};
}

#endif