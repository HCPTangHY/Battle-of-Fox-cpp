#pragma once
#include "BaseController.h"

namespace BoF {
class MapGenerater: public BaseController {
public:
    static Dict generate_grid();
    static Dict place_points();
    static Array get_boundary_points(int32_t width, int32_t height, double spacing);
    static Array get_jittered_grid(int32_t width, int32_t height, double spacing);
    static Dict calculate_voronoi(Array points, Array boundary);
    static int32_t find_grid_cell(double x, double y, Dict grid);

    // Heightmap generation
    static Array heightmap_generate(Dict graph,int32_t graphWidth, int32_t graphHeight);
    static Array heightmap_from_template(Dict graph, int32_t graphWidth, int32_t graphHeight,int32_t id);
    static Array heightmap_set_graph(Dict graph);

    static Array heightmap_add_step(Array heights, Dict grid,int32_t graphWidth, int32_t graphHeight, const std::vector<std::string>& elements);
    static Array heightmap_add_hill(Array heights,Dict grid, int32_t graphWidth, int32_t graphHeight,std::string count, std::string height, std::string rangeX, std::string rangeY);

    static Array heightmap_paths_calculate(Dict grid);
    static Array heightmap_connect_vertices(Dict cells, Dict vertices, int32_t start, int32_t h, Array used);
    static Array heightmap_simplify_line(Array chain, int32_t simplification);
};
}