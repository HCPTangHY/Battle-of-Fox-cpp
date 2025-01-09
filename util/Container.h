#pragma once

#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace BoF {
// 前向声明
class Dict;
class Array;
struct Undefined {};
// 可存储的值类型
using Value = std::variant<
    Undefined,
    int,
    size_t,
    double,
    std::string,
    bool,
    std::shared_ptr<Dict>,    // 使用Dict而不是Dictionary
    std::shared_ptr<Array>
>;
// 数组类 - 简化版本
class Array {
private:
    std::vector<Value> data;
    bool is_temp;
public:
    class ValueProxy {
        Value& val;
    public:
        ValueProxy(Value& v) : val(v) {}
        
        // 添加获取原始Value的方法
        const Value& getValue() const { return val; }
        Value& getValue() { return val; }
        // 对Array和Dict特化
        operator Array&() {
            return *std::get<std::shared_ptr<Array>>(val);
        }
        operator Dict&() {
            return *std::get<std::shared_ptr<Dict>>(val);
        }
        operator size_t() const {
            return std::get<size_t>(val);
        }

        // 类型转换运算符
        template<typename T>
        operator T() const {
            return static_cast<T>(std::visit([](const auto& v) -> double {
                using Type = std::decay_t<decltype(v)>;
                if constexpr (std::is_arithmetic_v<Type>) {
                    return static_cast<double>(v);
                }
                throw std::bad_variant_access();
            }, val));
        }

        // 赋值运算符
        template<typename T>
        ValueProxy& operator=(const T& v) {
            if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dict>) {
                val = std::make_shared<T>(v);  // 包装在shared_ptr中
            } else {
                val = v;  // 其他类型直接赋值
            }
            return *this;
        }
    };
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

    template<typename... Args>
    static Array make(Args&&... args) {
        Array arr;
        (arr.push(std::forward<Args>(args)), ...);
        return arr;
    }

    // 只保留用于访问的operator[]
    ValueProxy operator[](size_t index) {
        // 如果索引超出当前大小，扩展数组
        if (index >= data.size()) {
            data.resize(index + 1);  // 自动扩展到需要的大小
        }
        return ValueProxy(data[index]);
    }

    template<typename T>
    T get(size_t index) const {
        if (index >= data.size()) {
            throw std::out_of_range("Array index out of bounds");
        }
        
        try {
            if constexpr (std::is_same_v<T, Array>) {
                return *std::get<std::shared_ptr<Array>>(data[index]);
            } else if constexpr (std::is_same_v<T, Dict>) {
                return *std::get<std::shared_ptr<Dict>>(data[index]);
            } else {
                return std::get<T>(data[index]);
            }
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Type mismatch in array access");
        }
    }

    // 修改operator[]，使其委托给get方法
    template<typename T>
    T operator[](size_t index) const {
        return get<T>(index);
    }

    // 可以添加一个类似Dict的value()方法
    template<typename T>
    decltype(auto) value(size_t index) {
        if (index >= data.size()) {
            throw std::out_of_range("数组越界");
        }
        
        if constexpr (std::is_same_v<T, Dict> || std::is_same_v<T, Array>) {
            return *std::get<std::shared_ptr<T>>(data[index]);
        } else {
            return std::get<T>(data[index]);
        }
    }
    // 添加元素
    template<typename T>
    void push(const T& value) {
        if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dict>) {
            data.push_back(std::make_shared<T>(value));
        } else {
            data.push_back(value);
        }
    }
    void push(const ValueProxy& proxy) {
        data.push_back(proxy.getValue());
    }
    
    void push(const Value& value) {
        data.push_back(value);
    }

    void resize(size_t size) {
        size_t old_size = data.size();
        data.resize(size, Undefined{});  // 使用Undefined填充新位置
    }
    
    bool has(size_t index) const {
        return index < data.size() && 
               !std::holds_alternative<Undefined>(data[index]);
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

    Array& concat(const Array& other) {
        // 将other的所有元素添加到当前数组末尾
        data.insert(data.end(), other.data.begin(), other.data.end());
        return *this;
    }

    std::string toString() const {
        std::stringstream ss;
        ss.put('[');
        for (size_t i = 0; i < data.size(); i++) {
            std::visit([&ss](const auto& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Dict>>) {
                    ss << "<Dict:" << &*val << ">";
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Array>>) {
                    ss << val->toString();
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, Undefined>) {
                    ss << "undefined";
                }
                else {
                    ss << val;
                }
            }, data[i]);
            if (i < data.size() - 1) {
                ss << ", ";
            }
        }
        ss.put(']');
        return ss.str();
    }

    std::ostream& print(std::ostream& os) const {
        os.put('[');
        for (size_t i = 0; i < data.size(); i++) {
            std::visit([&os](const auto& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Dict>>) {
                    os << "<Dict:" << &*val << ">";
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::shared_ptr<Array>>) {
                    os << *val;
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, Undefined>) {
                    os << "undefined";
                }
                else {
                    os << val;
                }
            }, data[i]);
            if (i < data.size() - 1) {
                os << ", ";
            }
        }
        os.put(']');
        return os;
    }

    friend std::ostream& operator<<(std::ostream& os, const Array& arr) {
        os << '[';
        for (size_t i = 0; i < arr.data.size(); ++i) {
            std::visit([&os](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::shared_ptr<Dict>>) {
                    os << "<Dict:" << &*val << ">";
                }
                else if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                    os << *val;
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, Undefined>) {
                    os << "undefined";
                }
                else {
                    os << val;
                }
            }, arr.data[i]);
            if (i < arr.data.size() - 1) os << ", ";
        }
        os << ']';
        return os;
    }
};
// 字典类 - 简化版本
class Dict {
private:
    std::unordered_map<std::string, Value> data;
public:
class ValueProxy {
        Value& val;
    public:
        ValueProxy(Value& v) : val(v) {}
        
        // 获取原始Value
        operator const Value&() const { return val; }
        operator Value&() { return val; }
        
        // 其他类型的转换
        template<typename T>
        operator T() const {
            if constexpr (std::is_same_v<T, Dict> || std::is_same_v<T, Array>) {
                return *std::get<std::shared_ptr<T>>(val);
            } else if constexpr (!std::is_same_v<T, Value>) {
                return std::get<T>(val);
            }
        }

        std::shared_ptr<Array> operator->() const {
            return std::get<std::shared_ptr<Array>>(val);
        }
        Array::ValueProxy operator[](size_t index) {
            return std::get<std::shared_ptr<Array>>(val)->operator[](index);
        }
        // 赋值运算符
        template<typename T>
        ValueProxy& operator=(const T& v) {
            if constexpr (std::is_same_v<T, Array> || std::is_same_v<T, Dict>) {
                val = std::make_shared<T>(v);  // Wrap Array/Dict in shared_ptr
            } else {
                val = v;  // Direct assignment for other types
            }
            return *this;
        }
    };
    // 访问或创建元素
    ValueProxy operator[](const std::string& key) {
        return ValueProxy(data[key]);
    }

    // 设置值
    template<typename T>
    void set(const std::string& key, const ValueProxy& proxy) {
        // 直接存储底层的Value
        data[key] = proxy.val;
    }

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
        if constexpr (std::is_same_v<T, Dict> || std::is_same_v<T, Array>) {
            return *std::get<std::shared_ptr<T>>(it->second);
        } else {
            return std::get<T>(it->second);
        }
    }

    // 创建新的字典
    Dict& dict(const std::string& key) {
        auto d = std::make_shared<Dict>();
        data[key] = d;
        return *d;
    }

    // 创建新的数组
    Array& array(const std::string& key, size_t size = 0) {
        auto a = std::make_shared<Array>();
        a->resize(size);
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
}