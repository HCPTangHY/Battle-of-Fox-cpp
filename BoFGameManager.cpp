#include "BoFGameManager.h"
#include "controllers/MapGenerater.h"
#include "controllers/HeightmapColor.h"
#include <time.h>
#include "include/nlohmann/json.hpp"
#include "util/util.h"
#include "util/MapViewUtil.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/path2d.hpp>
#include <godot_cpp/classes/curve2d.hpp>
#include <godot_cpp/classes/geometry2d.hpp>
#include <godot_cpp/classes/color_rect.hpp>
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
    UtilityFunctions::print("grid generated");
    BoF::Array height = BoF::MapGenerater::heightmap_generate(grid,1000,1000);
    this->heights = height;
    UtilityFunctions::print(nlohmann::json(this->heights[1]).dump().c_str());
    // this->height_paths = BoF::MapGenerater::heightmap_paths_calculate(this->grid);
    UtilityFunctions::print(godot::String(nlohmann::json(height).dump().c_str()));
}

void BoFGameManager::draw_cells(Node *parent) {
    UtilityFunctions::print(godot::String("draw_cells"));

    const auto& cells = this->grid["cells"];
    // BoF::Array cellsI = cells["i"];
    for (int i = 0; i < cells.size(); ++i) {
        int cell_id = cells[i]["i"];
        godot::PackedVector2Array points = get_grid_polygon(cell_id);
        UtilityFunctions::print(cells[i]["h"].get<double>()/40);
        godot::Color color = godot::Color(cells[i]["h"].get<double>()/40,1,1,1);

        godot::Polygon2D *polygon = memnew(godot::Polygon2D);
        polygon->set_polygon(points);
        polygon->set_color(color);
        parent->add_child(polygon);
    }
    godot::Line2D *line = memnew(godot::Line2D);
    line->add_point(Vector2(0,0));
    line->add_point(Vector2(0,1000));
    parent->add_child(line);
    line = memnew(godot::Line2D);
    line->add_point(Vector2(0,0));
    line->add_point(Vector2(1000,0));
    parent->add_child(line);
    line = memnew(godot::Line2D);
    line->add_point(Vector2(1000,0));
    line->add_point(Vector2(1000,1000));
    parent->add_child(line);
    line = memnew(godot::Line2D);
    line->add_point(Vector2(0,1000));
    line->add_point(Vector2(1000,1000));
    parent->add_child(line);
}
godot::PackedVector2Array BoFGameManager::get_grid_polygon(int i) {
    PackedVector2Array points;
    const auto& cells = this->grid["cells"];
    BoF::Array cellsV = cells[i]["v"];
    const auto& vertices = this->grid["vertices"];

    for(int j = 0; j < cellsV.size(); j++) {
        int idx = cellsV[j];
        BoF::Array pos = vertices[idx]["p"];
        
        double x = pos[0];
        double y = pos[1];
        points.push_back(Vector2(x, y));
    }

    // 如果点数太少，可能不是一个有效的多边形
    // if(points.size() < 3) {
    //     return PackedVector2Array(); // 返回空数组，这个单元格将不会被渲染
    // }
    return points;
}

void BoFGameManager::draw_heightmap(Node* parent) {
    BoF::Array data = this->height_paths;
    for(int h = 0; h < 20; h++) {
        if (h==0) {
            BoFGameManager::render_base_layer(parent, "ocean");
        } else if (h < 20)
        {
            if(!data[h].is_null()) continue;
            BoF::Array height_paths = data[h];
            BoFGameManager::render_height_paths(parent, height_paths, h, "ocean");
        }
        if (h == 20) {
            BoFGameManager::render_base_layer(parent, "land");
        } else if (h >= 20 && h < 100) {
            if(!data[h].is_null()) continue;
            BoF::Array height_paths = data[h];
            BoFGameManager::render_height_paths(parent, height_paths, h, "land");
        }
    }
}

void BoFGameManager::render_base_layer(Node* parent, std::string type) {
    ColorRect* base = memnew(ColorRect);
    base->set_size(Vector2(1000, 1000));
    if(type == "ocean") {
        base->set_color(BoF::HeightmapColor::get_color(0, "ocean"));
    } else {
        base->set_color(BoF::HeightmapColor::get_color(20, "land"));
    }
    parent->add_child(base);
}

void BoFGameManager::render_height_paths(Node* parent, const BoF::Array paths, int height, std::string type) {
    godot::Color color = BoF::HeightmapColor::get_color(height, type);
    
    for(int i = 0; i < paths.size(); i++) {
        BoF::Array points = paths[i];

        godot::Path2D* path = memnew(Path2D);
        Ref<godot::Curve2D> curve(memnew(godot::Curve2D));

        for(int j = 0; j < points.size(); j++) {
            double x = points[j][0];
            double y = points[j][1];
            Vector2 point(x, y);
            curve->add_point(point);
        }
        curve->add_point(Vector2(points[0][0], points[1][1])); // 闭合路径
        path->set_curve(curve);

        PackedVector2Array smooth_points = curve->get_baked_points();
        
        Polygon2D* polygon = memnew(Polygon2D);
        polygon->set_polygon(smooth_points);
        polygon->set_color(color);
        
        // 添加terracing效果(如果需要)
        if(type == "land") {
            Polygon2D* shadow_polygon = memnew(Polygon2D);
            PackedVector2Array shadow_points = smooth_points;

            for(int j = 0; j < smooth_points.size(); j++) {
                shadow_points.set(j,shadow_points[j] + godot::Vector2(0.7, 1.4));
            }
            shadow_polygon->set_polygon(shadow_points);
            shadow_polygon->set_color(color.darkened(0.2));
            parent->add_child(shadow_polygon);
        }
        
        parent->add_child(path);
    }
}


void BoFGameManager::draw_features(Node* parent) {
    Node2D* featurePath = memnew(Node2D);
    featurePath->set_name("features");
    Node2D* landMask = memnew(Node2D);
    landMask->set_name("landMask");
    Node2D* waterMask = memnew(Node2D);
    waterMask->set_name("waterMask");
    Node2D* coastline = memnew(Node2D);
    coastline->set_name("coastline");

    parent->add_child(featurePath);
    parent->add_child(landMask);
    parent->add_child(waterMask);
    parent->add_child(coastline);
    for (const auto& feature : this->grid["features"]) {
        if (!feature.is_null() || feature["type"] == "ocean") continue;

        PackedVector2Array points = get_feature_polygon(feature);
        Polygon2D* polygon = memnew(Polygon2D);
        polygon->set_polygon(points);
        polygon->set_name("feature_" + String::num_int64(feature["i"].get<int>()));

        if(feature["type"] == "lake") {
            // 湖泊添加到陆地遮罩
            Polygon2D* mask_poly = Object::cast_to<Polygon2D>(polygon->duplicate());
            mask_poly->set_color(Color(0, 0, 0, 1)); // 黑色遮罩
            landMask->add_child(mask_poly);
            
            // 添加到湖泊组
            Node2D* lake_group = get_or_create_group(featurePath, feature["group"].get<std::string>().c_str());
            lake_group->add_child(polygon);
            
        } else {
            // 陆地添加到陆地和水体遮罩
            Polygon2D* land_poly = Object::cast_to<Polygon2D>(polygon->duplicate());
            land_poly->set_color(Color(1, 1, 1, 1)); // 白色遮罩
            landMask->add_child(land_poly);
            
            Polygon2D* water_poly = Object::cast_to<Polygon2D>(polygon->duplicate()); 
            water_poly->set_color(Color(0, 0, 0, 1));
            waterMask->add_child(water_poly);
            
            // 添加到海岸线组
            std::string coastline_group = feature["group"] == "lake_island" ? "lake_island" : "sea_island";
            Node2D* coast_group = get_or_create_group(coastline, coastline_group.c_str());
            coast_group->add_child(polygon);
        }
    }
}

Node2D* BoFGameManager::get_or_create_group(Node* parent, const String& name) {
    Node* existing = parent->get_node_or_null(name);
    if(existing) {
        return Object::cast_to<Node2D>(existing);
    }
    
    Node2D* group = memnew(Node2D);
    group->set_name(name);
    parent->add_child(group);
    return group;
}

PackedVector2Array BoFGameManager::get_feature_polygon(const BoF::Dict& feature) {
    BoF::Array points;
    const auto& vertices = nlohmann::json(feature)["vertices"];
    for(int i = 0; i < vertices.size(); i++) {
        int vertex_idx = vertices[i];
        const auto& pos = this->grid["vertices"]["p"][vertex_idx];
        points.push_back(BoF::Array({pos[0], pos[1]}));
    }
    points = BoF::simplify_points(points, 0.3);
    PackedVector2Array pointsPacked;
    for (const auto& point : points) {
        pointsPacked.push_back(Vector2(point[0], point[1]));
    }
    pointsPacked = clip_polygon(pointsPacked, 1.0);

    Ref<godot::Curve2D> curve = create_smooth_curve(pointsPacked);
    const int CURVE_POINTS = 32;
    PackedVector2Array smooth_points;
    float length = curve->get_baked_length();
    for(int i = 0; i <= CURVE_POINTS; i++) {
        float offset = length * i / CURVE_POINTS;
        smooth_points.push_back(curve->sample_baked(offset));
    }
    
    return smooth_points;
}
PackedVector2Array BoFGameManager::clip_polygon(const PackedVector2Array& points, float secure) {
    if(points.size() < 2) return points;
    
    float width = 1000.0; // 根据实际图形宽度调整
    float height = 1000.0; // 根据实际图形高度调整
    
    PackedVector2Array clip_rect;
    clip_rect.push_back(Vector2(secure, secure));
    clip_rect.push_back(Vector2(width - secure, secure));
    clip_rect.push_back(Vector2(width - secure, height - secure));
    clip_rect.push_back(Vector2(secure, height - secure));

    Geometry2D* geom = memnew(Geometry2D);
    TypedArray<PackedVector2Array> clipped = geom->clip_polygons(points, clip_rect);
    if(clipped.size() > 0) {
        return clipped[0];
    }
    return points;
}

Ref<godot::Curve2D> BoFGameManager::create_smooth_curve(const PackedVector2Array& points) {
    Ref<godot::Curve2D> curve;
    curve.instantiate();
    
    // 添加所有点作为控制点
    for(int i = 0; i < points.size(); i++) {
        curve->add_point(points[i]);
    }
    curve->add_point(points[0]);
    
    // 设置曲线为平滑
    for(int i = 0; i < curve->get_point_count(); i++) {
        Vector2 prev = curve->get_point_position((i - 1 + curve->get_point_count()) % curve->get_point_count());
        Vector2 next = curve->get_point_position((i + 1) % curve->get_point_count());
        Vector2 current = curve->get_point_position(i);
        
        Vector2 to_prev = (prev - current).normalized() * 20;
        Vector2 to_next = (next - current).normalized() * 20;
        
        curve->set_point_in(i, to_prev);
        curve->set_point_out(i, to_next);
    }
    
    return curve;
}

void BoFGameManager::read_json(const godot::String& json_str) {
    nlohmann::json json_data = nlohmann::json::parse(json_str.utf8().get_data());
    BoF::Dict cells = json_data.at("cells");
    this->grid = cells;
    // BoF::Dict gcells = this->grid["cells"];
}

void BoFGameManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_gameName", "gameName"), &BoFGameManager::_set_gameName);
    ClassDB::bind_method(D_METHOD("get_gameName"), &BoFGameManager::_get_gameName);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "gameName"), "_set_gameName", "_get_gameName");
    ClassDB::bind_method(D_METHOD("my_func", "from"), &BoFGameManager::my_func);
    ClassDB::bind_method(D_METHOD("map_ganerate"), &BoFGameManager::map_ganerate);
    ClassDB::bind_method(D_METHOD("draw_cells", "parent"), &BoFGameManager::draw_cells);
    ClassDB::bind_method(D_METHOD("read_json", "json_str"), &BoFGameManager::read_json);
}