#pragma once
#include "../include/nlohmann/json.hpp"

namespace BoF {

// 类型别名
using Array = nlohmann::json::array_t;  
using Dict = nlohmann::json::object_t;

// 辅助函数命名空间
namespace json {
    // 创建数组
    inline Array make_array() {
        return Array();
    }
    
    template<typename... Args>
    inline Array make_array(Args&&... args) {
        return Array{std::forward<Args>(args)...};
    }

    inline Array concat_arrays(const Array& arr1, const Array& arr2) {
        Array result = {};
        result.insert(result.end(), arr1.begin(), arr1.end());
        result.insert(result.end(), arr2.begin(), arr2.end());
        return result;
    }
    // 创建字典
    inline Dict make_dict() {
        return Dict();
    }
    
    // 从JSON字符串解析
    inline nlohmann::json parse(const std::string& str) {
        return nlohmann::json::parse(str);
    }
    
    // 转换为JSON字符串
    template<typename T>
    inline std::string stringify(const T& value) {
        return nlohmann::json(value).dump();
    }
}

// 可选:提供一些便利的转换函数
template<typename T>
inline T from_json(const nlohmann::json& j) {
    return j.get<T>();
}

template<typename T>
inline nlohmann::json to_json(const T& value) {
    return nlohmann::json(value);
}

} // namespace BoF
