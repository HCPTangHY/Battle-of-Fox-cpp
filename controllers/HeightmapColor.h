#pragma once

#include <vector>
#include <string>

#include <godot_cpp/variant/color.hpp>


namespace BoF {
class HeightmapColor {
public:
    static godot::Color get_color(float value, const std::string& scheme = "natural") {
        // 确保value在0-100范围内
        value = godot::CLAMP(value, 0.0f, 100.0f);
        
        // 调整value计算方式
        if (value < 20) {
            value = value - 5;
        }
        
        // 归一化到0-1
        float t = 1.0f - value / 100.0f;

        // 获取并插值颜色
        std::vector<godot::Color> colors = get_scheme_colors(scheme);
        return interpolate_color(colors, t);
    }

private:
    static std::vector<godot::Color> get_scheme_colors(const std::string& scheme) {
        if (scheme == "bright") {
            return {
                godot::Color(0.97, 0.98, 0.27), // 黄
                godot::Color(0.33, 0.87, 0.42), // 绿
                godot::Color(0.12, 0.57, 0.95)  // 蓝
            };
        }
        else if (scheme == "natural") {
            return {
                godot::Color(1, 1, 1),          // white
                godot::Color(0.93, 0.93, 0.80), // #EEEECC
                godot::Color(0.82, 0.71, 0.55), // tan
                godot::Color(0, 0.5, 0),        // green
                godot::Color(0, 0.5, 0.5)       // teal
            };
        }
        else if (scheme == "olive") {
            return {
                godot::Color(1, 1, 1),          // #ffffff
                godot::Color(0.81, 0.64, 0.55), // #cea48d
                godot::Color(0.84, 0.69, 0.52), // #d5b085
                godot::Color(0.05, 0.17, 0.10), // #0c2c19
                godot::Color(0.08, 0.07, 0.13)  // #151320
            };
        }
        
        // 默认返回natural方案
        return get_scheme_colors("natural");
    }

    static godot::Color interpolate_color(const std::vector<godot::Color>& colors, float t) {
        if (colors.size() == 1) return colors[0];
        if (t <= 0.0f) return colors[0];
        if (t >= 1.0f) return colors.back();

        float segment_length = 1.0f / (colors.size() - 1);
        int index = static_cast<int>(t / segment_length);
        if (index >= colors.size() - 1) index = colors.size() - 2;

        float local_t = (t - index * segment_length) / segment_length;
        return colors[index].lerp(colors[index + 1], local_t);
    }
};
}