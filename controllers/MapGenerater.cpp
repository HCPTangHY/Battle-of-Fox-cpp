#include "MapGenerater.h"
#include "../util/Container.h"

#include <stdint.h>
#include <cmath>

void MapGenerater::place_points() {
    const int32_t width = 1000;
    const int32_t height = 1000;

    const int32_t cellsDesired = 5000;
    const double spacing = round(((width * height) / cellsDesired)*100)/100;

    const Array boundary = MapGenerater::getBoundaryPoints(width, height,spacing);

}

Array MapGenerater::getBoundaryPoints(int32_t width, int32_t height, double spacing) {
    const int32_t offset = round(-1 * spacing);
    const double bSpacing = spacing * 2;
    const double w = width - offset * 2;
    const double h = height - offset * 2;
    const double numberX = std::ceil(w / bSpacing) - 1;
    const double numberY = std::ceil(h / bSpacing) - 1;
    Array points;

    for (double i = 0.5; i < numberX; i++) {
        int32_t x = std::ceil((w * i) / numberX + offset);
        points.push(Array()[x, offset]);
        points.push(Array()[x, h + offset]);
    }
    for (double i = 0.5; i < numberY; i++) {
        int32_t y = std::ceil((h * i) / numberY + offset);
        points.push(Array()[offset, y]);
        points.push(Array()[w + offset, y]);
    }
    return points;
}