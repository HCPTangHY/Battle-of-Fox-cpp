// GameData.h
#pragma once  // 防止头文件被重复包含
#include <string>
#include <map>
#include <any>
#include <memory>  // 用于smart pointers
#include <variant>  // 用于存储多种类型
#include <vector>

// variant是一个可以存储多种类型的容器
// 这里定义了一个可以存储int,double,bool,string的类型
using ValueType = std::variant<int, double, bool, std::string>;

// 前向声明DataNode类，这样其他类可以使用DataNode指针而不需要完整的类定义
class DataNode;

// 定义一个指向DataNode的智能指针类型
// shared_ptr是一种会自动管理内存的指针，当没有人使用这个对象时会自动删除它
using DataNodePtr = std::shared_ptr<DataNode>;

// 数据节点类，用于存储游戏中的各种数据
class DataNode {
public:
    // 存储基础类型的值（如数字、字符串等）
    // map是一个键值对容器，这里用string作为键，ValueType作为值
    std::map<std::string, ValueType> values;
    
    // 存储子节点，允许创建树形结构
    // 比如 game/player/stats 这样的数据结构
    std::map<std::string, DataNodePtr> children;

    // 设置值的模板函数
    // template允许这个函数处理不同类型的数据
    // 例如: setValue("speed", 1.0) 或 setValue("name", "player1")
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        values[key] = value;
    }

    // 获取值的模板函数
    // 如果值不存在或类型不匹配，返回默认值
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        auto it = values.find(key);  // 查找键
        if (it != values.end()) {    // 如果找到了
            try {
                // 尝试获取正确类型的值
                return std::get<T>(it->second);
            } catch (...) {  // 如果类型不匹配
                return defaultValue;
            }
        }
        return defaultValue;  // 如果没找到键
    }

    // 创建一个新的子节点
    DataNodePtr createChild(const std::string& key) {
        auto node = std::make_shared<DataNode>();  // 创建新节点
        children[key] = node;  // 存储到子节点map中
        return node;
    }

    // 获取子节点，如果不存在返回nullptr
    DataNodePtr getChild(const std::string& key) {
        auto it = children.find(key);
        if (it != children.end()) {
            return it->second;
        }
        return nullptr;
    }

    // 删除子节点
    void removeChild(const std::string& key) {
        children.erase(key);  // 从map中删除
    }
};

// 游戏数据管理类，使用单例模式
class GameData {
private:
    static GameData* instance;  // 单例实例指针
    GameData();  // 私有构造函数，防止直接创建实例

    DataNodePtr root;  // 根节点

public:
    // 获取单例实例的静态方法
    static GameData* getInstance();

    // 初始化和清理
    bool initialize();
    void cleanup();

    // 获取根节点
    DataNodePtr getRoot() { return root; }
    
    // 通过路径设置值的便捷方法
    // 例如: setValue("game/player/health", 100)
    template<typename T>
    void setValue(const std::string& path, const T& value) {
        auto node = getOrCreateNode(path);
        if (node) {
            node->setValue(getLastKey(path), value);
        }
    }

    // 通过路径获取值的便捷方法
    // 例如: getValue("game/player/health", 0)
    template<typename T>
    T getValue(const std::string& path, const T& defaultValue) const {
        auto node = findNode(path);
        if (node) {
            return node->getValue(getLastKey(path), defaultValue);
        }
        return defaultValue;
    }

    // 创建新节点
    DataNodePtr createNode(const std::string& path) {
        return getOrCreateNode(path);
    }

    // 删除节点
    void removeNode(const std::string& path) {
        auto parentPath = getParentPath(path);
        auto parent = findNode(parentPath);
        if (parent) {
            parent->removeChild(getLastKey(path));
        }
    }

private:
    // 根据路径获取或创建节点
    DataNodePtr getOrCreateNode(const std::string& path);
    // 查找节点
    DataNodePtr findNode(const std::string& path) const;
    // 获取父节点路径
    std::string getParentPath(const std::string& path) const;
    // 获取路径中的最后一个键名
    std::string getLastKey(const std::string& path) const;
};
