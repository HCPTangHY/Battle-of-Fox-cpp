#pragma once
#include "util/Container.h"

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/curve2d.hpp>

using namespace godot;

class BoFGameManager : public Node {
    GDCLASS(BoFGameManager, Node);

    public:
        godot::String gameName = "Sionnachia: Prequel";
        BoF::Dict grid;
        BoF::Array heights;
        BoF::Array height_paths;

        godot::String _get_gameName() {return gameName;};
        void _set_gameName(godot::String p_gameName) {this->gameName = p_gameName;};
        void my_func(godot::String from);

        void map_ganerate();
        void draw_cells(godot::Node *parent);
        godot::PackedVector2Array get_grid_polygon(int cell_id);

        void draw_heightmap(Node* parent);
        void render_base_layer(Node* parent, std::string type);
        void render_height_paths(Node* parent, const BoF::Array paths, int height, std::string type);

        void draw_features(Node* parent);
        Node2D* get_or_create_group(Node* parent, const String& name);
        PackedVector2Array get_feature_polygon(const BoF::Dict& feature);
        PackedVector2Array clip_polygon(const PackedVector2Array& points, float secure);
        Ref<godot::Curve2D> create_smooth_curve(const PackedVector2Array& points);

        void read_json(const godot::String& json_str);
    protected:
        static void _bind_methods();


};