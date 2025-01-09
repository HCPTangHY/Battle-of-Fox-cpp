#pragma once
#include "util/Container.h"

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class BoFGameManager : public Node {
    GDCLASS(BoFGameManager, Node);

    public:
        godot::String gameName = "Sionnachia: Prequel";
        BoF::Dict grid;

        godot::String _get_gameName() {return gameName;};
        void _set_gameName(godot::String p_gameName) {this->gameName = p_gameName;};
        void my_func(godot::String from);

        void map_ganerate();
        void draw_cells(godot::Node *parent);
        godot::PackedVector2Array get_grid_polygon(int cell_id);

    protected:
        static void _bind_methods();


};