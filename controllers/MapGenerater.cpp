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

    // result.set("seed",seed);
    return {
        {"spacing", pointsInfo["spacing"]},
        {"cellsDesired", pointsInfo["cellsDesired"]},
        {"boundary", pointsInfo["boundary"]},
        {"points", pointsInfo["points"]},
        {"cellsX", pointsInfo["cellsX"]},
        {"cellsY", pointsInfo["cellsY"]},
        {"cells", voronoiInfo["cells"]},
        {"vertices", voronoiInfo["vertices"]}
    };
}

Dict MapGenerater::place_points() {
    const int32_t width = 800;
    const int32_t height = 600;

    const int32_t cellsDesired = 5000;
    const double spacing = rn(sqrt((width * height) / cellsDesired),2);

    const Array boundary = MapGenerater::get_boundary_points(width, height,spacing);
    const Array points = MapGenerater::get_jittered_grid(width, height, spacing);
    const double cellsX = floor((width + 0.5 * spacing - 1e-10) / spacing);
    const double cellsY = floor((height + 0.5 * spacing - 1e-10) / spacing);

    return {
        {"spacing", spacing},
        {"cellsDesired", cellsDesired},
        {"boundary", boundary},
        {"points", points},
        {"cellsX", cellsX},
        {"cellsY", cellsY}
    };

}

Array MapGenerater::get_boundary_points(int32_t width, int32_t height, double spacing) {
    const double offset = rn(-1 * spacing);
    std::cout << "offset" << offset << std::endl;
    const double bSpacing = spacing * 2;
    const double w = width - offset * 2;
    const double h = height - offset * 2;
    const double numberX = std::ceil(w / bSpacing) - 1;
    const double numberY = std::ceil(h / bSpacing) - 1;
    Array points;

    for (double i = 0.5; i < numberX; i++) {
        double x = std::ceil((w * i) / numberX + offset);
        points.push_back({x, offset});
        points.push_back({x, h + offset});
    }
    
    for (double i = 0.5; i < numberY; i++) {
        double y = std::ceil((h * i) / numberY + offset);
        points.push_back({offset, y});
        points.push_back({w + offset, y});
    }
    return points;
}

Array MapGenerater::get_jittered_grid(int32_t width, int32_t height, double spacing) {
    const double radius = spacing / 2;
    const double jittering = radius * 0.9; // max deviation
    const double doubleJittering = jittering * 2;
    auto jitter = [doubleJittering, jittering]() {
       return rand() / double(RAND_MAX) * doubleJittering - jittering;
    };
    
    Array points;
    for (double y = radius; y < height; y += spacing) {
        for (double x = radius; x < width; x += spacing) {
            double xj = std::min<double>(rn(x + jitter(),2), width);
            double yj = std::min<double>(rn(y + jitter(),2) , height);
            points.push_back({xj, yj});
        }
    }
  return points;
}

Dict MapGenerater::calculate_voronoi(Array points, Array boundary) {
    Array allpoints = json::concat_arrays(points, boundary);
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
    cells["i"] = Array(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        cells["i"][i] = i;
    }
    Dict vertices = voronoi.vertices;

    return {
        {"cells", cells},
        {"vertices", vertices}
    };
}

int32_t MapGenerater::find_grid_cell(double x, double y, Dict grid) {
    double spacing = grid["spacing"];
    double cellsX = grid["cellsX"];
    double cellsY = grid["cellsY"];
    return (
        floor(std::min<double>(x / spacing, cellsY - 1)) * cellsX +
        floor(std::min<double>(y / spacing, cellsX - 1))
    );
}

// ## 高度图生成

Array MapGenerater::heightmap_generate(Dict graph,int32_t graphWidth, int32_t graphHeight) {
    Array heights = MapGenerater::heightmap_from_template(graph,graphWidth, graphHeight, 1);
    return heights;
}

Array MapGenerater::heightmap_from_template(Dict graph,int32_t graphWidth, int32_t graphHeight, int32_t id) {

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
    Array heights = graphArr[0];
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

        heights = MapGenerater::heightmap_add_step(heights, graphArr[3], graphWidth, graphHeight, elements);
    }
    return heights;
}

Array MapGenerater::heightmap_set_graph(Dict graph) {
    int32_t cellsDesired = graph["cellsDesired"];
    Dict cells = graph["cells"];
    Array points = graph["points"];
    Array cellsH = cells.count("h") ? cells["h"].get<Array>() : Array();
    Array newArray = Array(points.size(),0);
    Array heights = cells.count("h") ? cellsH : newArray;
    double blobPower = 0.98;
    double linePower = 0.81;
    Dict grid = graph;
    return {
        heights,blobPower,linePower,grid
    };
};

Array MapGenerater::heightmap_add_step(Array heights, Dict grid,int32_t graphWidth, int32_t graphHeight, const std::vector<std::string>& elements) {
    if (elements.empty()) {
        throw std::runtime_error("No tool specified");
    }
    std::string tool = elements[0];
    
    if (tool == "Hill") {
        auto getParam = [&elements](size_t index) -> std::string {
            if (index < elements.size()) {
                return elements[index];
            }
            return "";  // 或者其他默认值
        };
        
        return heightmap_add_hill(heights, grid, graphWidth, graphHeight,
                                getParam(1),
                                getParam(2),
                                getParam(3),
                                getParam(4));
    }
    return heights;
}

Array MapGenerater::heightmap_add_hill(Array heights, Dict grid, int32_t graphWidth, int32_t graphHeight, std::string count, std::string height, std::string rangeX, std::string rangeY) {
    int32_t countNum = get_number_in_range(count);
    while (countNum > 0)
    { 
        Array change = Array(heights.size());
        int32_t limit = 0;
        int32_t start;
        int32_t h = lim(get_number_in_range(height));

        do
        {
            double x = get_point_in_range(rangeX,graphWidth);
            double y = get_point_in_range(rangeY,graphHeight);
            start = MapGenerater::find_grid_cell(x, y, grid);
            limit++;
        } while (heights[start].get<int32_t>() + h > 90 && limit < 50);
        
        change[start] = h;
        Array queue = {start};
        while (queue.size())
        {
            auto q = queue.front();
            queue.erase(queue.begin());

            Dict cells = grid["cells"];
            Array cellsI = cells["i"];
            for (int i = 0; i < cellsI.size(); i++)
            {
                if (change[i].is_null()) continue;
                double changeQ = change[q];
                change[i] = pow(changeQ, 0.98) * ((static_cast<double>(rand()) / RAND_MAX) * 0.2 + 0.9);
                int32_t changeI = change[i];
                if (changeI > 1) queue.push_back(i);
            }
        }
        Array heightsNew = Array();
        for (size_t i = 0; i < heights.size(); i++) {
            double value = heights[i];
            heightsNew.push_back(static_cast<int>(lim(i + value)));
        }
        heights = heightsNew;
        countNum--;
    }
    
    return heights;
}

Array MapGenerater::heightmap_paths_calculate(Dict grid) {
    Array used = Array();
    used.resize(grid["cells"]["i"].size());
    Dict cells = grid["cells"];
    Dict vertices = grid["vertices"];
    Array paths = Array(101);
    // 海洋
    int skip = 1;

    int currentLayer = 0;
    Array heights = grid["cells"]["h"];
    for (int i = 0; i < heights.size(); i++) {
        int h = heights[i];
        if (h > currentLayer) currentLayer += skip;
        if (h < currentLayer) continue;
        if (currentLayer >= 20) break;
        if (used[i].is_null()) continue;

        bool onborder = std::any_of(cells["c"][i].begin(), cells["c"][i].end(),
            [&cells, h](const auto& n) { return cells["h"][n] < h; });
        if (onborder) continue;
        int32_t vertex = std::find_if(cells["v"][i].begin(), cells["v"][i].end(),
            [&vertices, &cells, h, i](const auto& v) {
                return std::any_of(vertices["c"][v].begin(), vertices["c"][v].end(),
                    [&cells, h](const auto& j) { 
                        return cells["h"][j] < h; 
                    });
            }) - cells["v"][i].begin();
        Array chain = MapGenerater::heightmap_connect_vertices(cells, vertices, vertex, h, used);
        if (chain.size() < 3) continue;

        Array points = MapGenerater::heightmap_simplify_line(chain, 0);
        Array points_new = Array();
        for (int j = 0; j < points.size(); j++) {
            int32_t v = points[j];
            points_new.push_back(vertices["p"][v]);
        }
        points = points_new;
        if (!paths[h].is_null()) paths[h] = Array();
        Array height_paths = paths[h];
        height_paths.push_back(points);
        paths[h] = height_paths;
    }
    // 陆地
    int skip2 = 1;

    int currentLayer2 = 20;
    for (int i = 0; i < heights.size(); i++) {
        int h = heights[i];
        if (h > currentLayer2) currentLayer2 += skip2;
        if (h < currentLayer2) continue;
        if (currentLayer2 >= 100) break;
        if (used[i].is_null()) continue;

        bool onborder = std::any_of(cells["c"][i].begin(), cells["c"][i].end(),
            [&cells, h](const auto& n) { return cells["h"][n] < h; });
        if (onborder) continue;
        int32_t startVertex = std::find_if(cells["v"][i].begin(), cells["v"][i].end(),
            [&vertices, &cells, h, i](const auto& v) {
                return std::any_of(vertices["c"][v].begin(), vertices["c"][v].end(),
                    [&cells, h](const auto& j) { 
                        return cells["h"][j] < h; 
                    });
            }) - cells["v"][i].begin();
        Array chain = MapGenerater::heightmap_connect_vertices(cells, vertices, startVertex, h, used);
        if (chain.size() < 3) continue;

        Array points = MapGenerater::heightmap_simplify_line(chain, 0);
        Array points_new = Array();
        for (int j = 0; j < points.size(); j++) {
            int32_t v = points[j];
            points_new.push_back(vertices["p"][v]);
        }
        points = points_new;
        if (!paths[h].is_null()) paths[h] = Array();
        Array height_paths = paths[h];
        height_paths.push_back(points);
        paths[h] = height_paths;
    }

    return paths;
}

Array MapGenerater::heightmap_connect_vertices(Dict cells, Dict vertices, int32_t start, int32_t h, Array used) {
    int32_t max_iterations = vertices["c"].size();

    int32_t n = cells["i"].size();
    Array chain;
    for (int32_t i = 0, current = start; i == 0 || (current!= start && i<max_iterations); i++) {
        int32_t prev = chain.empty()? 0 : chain[chain.size() - 1].get<int32_t>();
        chain.push_back(current);
        Array c = vertices["c"][current];
        Array c_filter;
        for (int32_t j = 0; j < c.size(); j++) {
            if (cells["h"][c[j]] == h) {
                c_filter.push_back(c[j]);
            }
        }
        for (int32_t j = 0; j < c_filter.size(); j++) {
            used[j] = 1;
        }
        bool c0 = c[0] >= n || cells["h"][c[0]] < h;
        bool c1 = c[1] >= n || cells["h"][c[1]] < h;
        bool c2 = c[2] >= n || cells["h"][c[2]] < h;
        Array v = vertices["v"][current];
        if (v[0] != prev && c0 != c1) current = v[0];
        else if (v[1] != prev && c1 != c2) current = v[1];
        else if (v[2] != prev && c2 != c0) current = v[2];
        int32_t chain_last = chain.empty()? 0 : chain[chain.size() - 1].get<int32_t>();
        if (current == chain_last) break;
    }
    return chain;
}

Array MapGenerater::heightmap_simplify_line(Array chain, int32_t simplification = 0) {
    if (!simplification) return chain;
    int32_t n = simplification + 1;
    Array chain_new;
    for (int32_t i = 0; i < chain.size(); i += n) {
        if (chain[i].get<int32_t>()%n == 0) {
            chain_new.push_back(chain[i]);
        }
    }
    return chain_new;
}

};