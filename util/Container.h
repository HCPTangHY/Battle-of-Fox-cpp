#pragma once

#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

// 前向声明
class Dict;
class Array;

// 可存储的值类型
using Value = std::variant<
    int,
    double,
    std::string,
    bool,
    std::shared_ptr<Dict>,    // 使用Dict而不是Dictionary
    std::shared_ptr<Array>
>;
class ArrayBuilder {
    std::vector<Value> values;
public:
    ArrayBuilder(Value first) {
        values.push_back(first);
    }
    
    ArrayBuilder& operator,(Value next) {
        values.push_back(next);
        return *this;
    }
    
    operator Array() {
        Array arr;
        for (const auto& v : values) {
            arr.push(v);
        }
        return arr;
    }
};
// 数组类 - 简化版本
class Array {
private:
    std::vector<Value> data;

public:
    ArrayBuilder operator[](Value first) {
        return ArrayBuilder(first);
    }
    // 添加元素
    void push(const Value& value) {
        data.push_back(value);
    }

    // 获取元素
    Value& operator[](size_t index) {
        if (index >= data.size()) {
            throw std::out_of_range("数组越界");
        }
        return data[index];
    }

    // 获取具体类型的值
    template<typename T>
    T get(size_t index) {
        if (index >= data.size()) {
            throw std::out_of_range("数组越界");
        }
        return std::get<T>(data[index]);
    }

    // 获取大小
    size_t size() const {
        return data.size();
    }

    // 删除元素
    void remove(size_t index) {
        if (index < data.size()) {
            data.erase(data.begin() + index);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& arr);
};
inline std::ostream& operator<<(std::ostream& os, const Array& arr) {
    os.put('[');
    for (size_t i = 0; i < arr.data.size(); i++) {
        std::visit([&os](const auto& val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Dict>>) {
                os << "<Dict:" << &*val << ">";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Array>>) {
                os << *val;
            }
            else {
                os << val;
            }
        }, arr.data[i]);
        if (i < arr.data.size() - 1) {
            os << ", ";
        }
    }
    os.put(']');
    return os;
}

// 字典类 - 简化版本
class Dict {
private:
    std::unordered_map<std::string, Value> data;

public:
    // 访问或创建元素
    Value& operator[](const std::string& key) {
        return data[key];
    }

    // 设置值
    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = value;
    }

    // 获取值
    template<typename T>
    T get(const std::string& key) {
        auto it = data.find(key);
        if (it == data.end()) {
            throw std::out_of_range("键不存在");
        }
        return std::get<T>(it->second);
    }

    // 创建新的字典
    Dict& dict(const std::string& key) {
        auto d = std::make_shared<Dict>();
        data[key] = d;
        return *d;
    }

    // 创建新的数组
    Array& array(const std::string& key) {
        auto a = std::make_shared<Array>();
        data[key] = a;
        return *a;
    }

    // 检查键是否存在
    bool has(const std::string& key) const {
        return data.find(key) != data.end();
    }

    // 删除键
    void remove(const std::string& key) {
        data.erase(key);
    }

    // 获取全部键
    Array keys() const {
        Array a;
        for (const auto& [key, value] : data) {
            a.push(key);
        }
        return a;
    }
};
