#pragma once
#include "BaseController.h"

class MapGenerater: public BaseController {
    static void generate_grid();
    static void place_points();
    static Array getBoundaryPoints(int32_t width, int32_t height, double spacing);

};
