#include "../include/delaunator.hpp"

#include "MapGenerater.h"
#include "../util/Container.h"
#include "../util/Voronoi.h"
#include "../util/util.h"

#include <stdint.h>
#include <cmath>
#include <numeric>
#include <stdio.h>

namespace BoF {
Dict MapGenerater::generate_grid() {
    // std::mt19937 rng(seed);
    Dict pointsInfo = MapGenerater::place_points();
    Dict voronoiInfo = MapGenerater::calculate_voronoi(pointsInfo["points"], pointsInfo["boundary"]);

    Dict result;
    result.set("spacing", pointsInfo["spacing"]);
    result.set("cellsDesired", pointsInfo["cellsDesired"]);
    result.set("boundary", pointsInfo["boundary"]);
    result.set("points", pointsInfo["points"]);
    result.set("cellsX", pointsInfo["cellsX"]);
    result.set("cellsY", pointsInfo["cellsY"]);
    result.set("cells", voronoiInfo["cells"]);
    result.set("vertices", voronoiInfo["vertices"]);
    // result.set("seed",seed);
    return result;
}

Dict MapGenerater::place_points() {
    const int32_t width = 1000;
    const int32_t height = 1000;

    const int32_t cellsDesired = 1000;
    const double spacing = round(sqrt((width * height) / cellsDesired)*100)/100;

    const Array boundary = MapGenerater::get_boundary_points(width, height,spacing);
    const Array points = MapGenerater::get_jittered_grid(width, height, spacing);
    const double cellsX = floor((width + 0.5 * spacing - 1e-10) / spacing);
    const double cellsY = floor((height + 0.5 * spacing - 1e-10) / spacing);

    Dict result;
    result["spacing"] = spacing;
    result["cellsDesired"] = cellsDesired;
    result["boundary"] = std::make_shared<Array>(boundary);
    result["points"] = std::make_shared<Array>(points);
    result["cellsX"] = cellsX;
    result["cellsY"] = cellsY;
    return result;

}

Array MapGenerater::get_boundary_points(int32_t width, int32_t height, double spacing) {
    const double offset = round(-1 * spacing);
    const double bSpacing = spacing * 2;
    const double w = width - offset * 2;
    const double h = height - offset * 2;
    const double numberX = std::ceil(w / bSpacing) - 1;
    const double numberY = std::ceil(h / bSpacing) - 1;
    Array points;

    for (double i = 0.5; i < numberX; i++) {
        int32_t x = std::ceil((w * i) / numberX + offset);
        points.push(Array::make(x, offset));
        points.push(Array::make(x, h + offset));
    }
    for (double i = 0.5; i < numberY; i++) {
        int32_t y = std::ceil((h * i) / numberY + offset);
        points.push(Array::make(offset, y));
        points.push(Array::make(w + offset, y));
    }
    return points;
}

Array MapGenerater::get_jittered_grid(int32_t width, int32_t height, double spacing) {
    const double radius = spacing / 2;
    const double jittering = radius * 0.9; // max deviation
    const double doubleJittering = jittering * 2;
    auto jitter = [doubleJittering, jittering]() {
       return (double)rand() / RAND_MAX * doubleJittering - jittering;
    };
    
    Array points;
    for (double y = radius; y < height; y += spacing) {
        for (double x = radius; x < width; x += spacing) {
            double xj = std::min<double>(round(x + jitter() * 100) / 100.0, width);
            double yj = std::min<double>(round(y + jitter() * 100) / 100 , height);            points.push(Array::make(xj, yj));
        }
    }
  return points;
}

Dict MapGenerater::calculate_voronoi(Array points, Array boundary) {
    Array allpoints = points.concat(boundary);
    std::vector<double> coords;
    coords.reserve(allpoints.size() * 2);  // 预分配空间

    for(size_t i = 0; i < points.size(); i++) {
        // 获取内层数组
        Array point = points[i];
        double x = point[0];
        double y = point[1];
        coords.push_back(x);
        coords.push_back(y);
    }

    delaunator::Delaunator delaunay(coords);

    Voronoi voronoi(std::make_shared<delaunator::Delaunator>(delaunay),allpoints,points.size());

    Dict cells = voronoi.cells;
    cells.array("i",points.size());
    for(size_t i = 0; i < points.size(); i++) {
        cells["i"][i] = i;
    }
    Dict vertices = voronoi.vertices;
    Dict result;
    result.set("cells",std::make_shared<Dict>(cells));
    result.set("vertices",std::make_shared<Dict>(vertices));

    return result;
}

// ## 高度图生成

Array MapGenerater::heightmap_generate(Dict graph) {
    Array heights = MapGenerater::heightmap_from_template(graph, 1);
    return heights;
}

Array MapGenerater::heightmap_from_template(Dict graph, int32_t id) {

    // 获取模板字符串
    const std::string& templateString = "Hill 1\n90-100 44-56 40-60\nMultiply 0.8 50-100 0 0\nRange 1.5 30-55 45-55 40-60\nSmooth 3 0 0 0\nHill 1.5 35-45 25-30 20-75\nHill 1 35-55 75-80 25-75\nHill 0.5 20-25 10-15 20-25\nMask 3 0 0 0";
    
    // 分割成步骤
    std::vector<std::string> steps = split(templateString, '\n');
    
    // 检查步骤是否为空
    if (steps.empty()) {
        throw std::runtime_error(
            "Heightmap template: no steps. Template: " + std::to_string(id) + 
            ". Steps: " + std::to_string(steps.size())
        );
    }

    // 设置graph
    Array graphArr = MapGenerater::heightmap_set_graph(graph);

    // 处理每个步骤
    for (const auto& step : steps) {
        // 分割步骤为元素
        std::vector<std::string> elements = split(step, ' ');
        
        // 检查元素数量
        if (elements.size() < 2) {
            throw std::runtime_error(
                "Heightmap template: steps < 2. Template: " + std::to_string(id) + ". Step: " + step
            );
        }

        // 调用addStep
        // addStep(elements);
    }
    return graphArr[0];
}

Array MapGenerater::heightmap_set_graph(Dict graph) {
    int32_t cellsDesired = graph["cellsDesired"];
    Dict cells = graph["cells"];
    Array points = graph["points"];
    Array heights = cells.has("h") ? cells["h"] : Array();
    double blobPower = 0.98;
    double linePower = 0.81;
    Dict grid = graph;
    Array result;
    result.push(heights);
    result.push(blobPower);
    result.push(linePower);
    result.push(std::make_shared<Dict>(grid));
    return result;
};



};