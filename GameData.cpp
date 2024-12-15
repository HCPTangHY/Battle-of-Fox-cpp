// GameData.cpp
#include "GameData.h"
#include <iostream>
#include <sstream>  // 用于字符串流处理

// 初始化静态成员变量
GameData* GameData::instance = nullptr;

// 获取单例实例
GameData* GameData::getInstance() {
    if (instance == nullptr) {
        instance = new GameData();
    }
    return instance;
}

// 构造函数
GameData::GameData() {
    // 创建根节点
    root = std::make_shared<DataNode>();
}

// 初始化游戏数据
bool GameData::initialize() {
    // 设置一些初始值
    setValue("game/timeSpeed", 1.0);
    setValue("game/currentTick", 0);
    setValue("settings/maxSpeed", 5.0);
    return true;
}

// 清理数据
void GameData::cleanup() {
    // 清空所有数据
    root->children.clear();
    root->values.clear();
}

// 根据路径获取或创建节点
DataNodePtr GameData::getOrCreateNode(const std::string& path) {
    // 创建字符串流用于分割路径
    std::istringstream iss(path);
    std::string key;
    DataNodePtr current = root;

    // 按'/'分割路径并逐级创建或获取节点
    while (std::getline(iss, key, '/')) {
        if (key.empty()) continue;
        
        auto child = current->getChild(key);
        if (!child) {
            child = current->createChild(key);
        }
        current = child;
    }
    return current;
}

// 查找节点
DataNodePtr GameData::findNode(const std::string& path) const {
    std::istringstream iss(path);
    std::string key;
    DataNodePtr current = root;

    // 按'/'分割路径并逐级查找节点
    while (std::getline(iss, key, '/')) {
        if (key.empty()) continue;
        
        current = current->getChild(key);
        if (!current) break;
    }
    return current;
}

// 获取父节点路径
std::string GameData::getParentPath(const std::string& path) const {
    // 查找最后一个'/'的位置
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) return "";
    return path.substr(0, pos);
}

// 获取路径中的最后一个键名
std::string GameData::getLastKey(const std::string& path) const {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}
