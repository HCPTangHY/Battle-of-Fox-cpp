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

    // Heightmap generation
    static Array heightmap_generate(Dict graph);
    static Array heightmap_from_template(Dict graph, int32_t id);
    static Array heightmap_set_graph(Dict graph);
};
}