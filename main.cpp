#include "util/Container.h"
#include <iostream>

int main() {
    Dict root;  // 创建根字典

    // 创建嵌套结构
    auto& game = root.dict("game");
    game.set("name", "我的游戏");
    game.set("version", 1.0);
    game.set("active", true);

    // 创建并操作数组
    auto& scores = game.array("scores");
    scores.push(100);
    scores.push(200);
    scores.push(300);

    // 创建玩家数据
    auto& players = game.array("players");
    
    auto player1 = std::make_shared<Dict>();
    player1->set("name", "玩家1");
    player1->set("level", 10);
    players.push(player1);

    // 读取数据
    try {
        std::string name = game.get<std::string>("name");
        double version = game.get<double>("version");
        bool active = game.get<bool>("active");
        int firstScore = scores.get<int>(0);

        std::cout << "游戏名称: " << name << std::endl;
        std::cout << "版本: " << version << std::endl;
        std::cout << "是否激活: " << (active ? "是" : "否") << std::endl;
        std::cout << "第一个分数: " << firstScore << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }

    return 0;
}
