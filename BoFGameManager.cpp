#include "BoFGameManager.h"
#include "controllers/MapGenerater.h"
#include <time.h>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/engine.hpp>

void BoFGameManager::my_func(godot::String from) {
    BoF::Array arr;
    UtilityFunctions::print("Hello, world! form BoF");
    UtilityFunctions::print(static_cast<godot::String>(typeid(arr).name()));
    UtilityFunctions::print(from);
}

void BoFGameManager::map_ganerate() {
    BoF::Dict grid = BoF::MapGenerater::generate_grid();
    this->grid = grid;
    BoF::Array height = BoF::MapGenerater::heightmap_generate(grid);
}

void BoFGameManager::draw_cells(Node *parent) {
    UtilityFunctions::print(godot::String("draw_cells"));

    BoF::Dict cells = this->grid["cells"];
    BoF::Array cellsI = cells["i"];
    for (int i = 0; i < cellsI.size(); ++i) {
        int cell_id = cellsI[i];
        godot::PackedVector2Array points = get_grid_polygon(cell_id);
        godot::Color color = godot::Color(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);

        godot::Polygon2D *polygon = memnew(godot::Polygon2D);
        polygon->set_polygon(points);
        polygon->set_color(color);
        parent->add_child(polygon);
    }
}
godot::PackedVector2Array BoFGameManager::get_grid_polygon(int i) {
    PackedVector2Array points;
    BoF::Dict cells = this->grid["cells"];
    BoF::Array cellsV = cells["v"];
    BoF::Array cellsVi = cellsV[i];

    for(int j = 0; j < cellsVi.size(); j++) {
        BoF::Dict vertices = this->grid["vertices"];
        BoF::Array verticesP = vertices["p"];
        int idx = cellsVi[j];
        BoF::Array pos = verticesP[idx];
        UtilityFunctions::print(godot::String(std::to_string(pos.get<double>(0)*10).c_str()));
        points.push_back(godot::Vector2(pos.get<double>(0)*100, pos.get<double>(1)*100));
    }
    return points;
}

void BoFGameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_gameName", "gameName"), &BoFGameManager::_set_gameName);
    ClassDB::bind_method(D_METHOD("get_gameName"), &BoFGameManager::_get_gameName);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "gameName"), "_set_gameName", "_get_gameName");
    ClassDB::bind_method(D_METHOD("my_func", "from"), &BoFGameManager::my_func);
    ClassDB::bind_method(D_METHOD("map_ganerate"), &BoFGameManager::map_ganerate);
    ClassDB::bind_method(D_METHOD("draw_cells", "parent"), &BoFGameManager::draw_cells);
}