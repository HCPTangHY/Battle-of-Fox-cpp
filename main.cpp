#include "util/Container.h"
#include "util/GameData.h"
#include "models/Country.h"
#include <iostream>

int main() {
    system("chcp 65001");
    Dict root;  // 创建根字典

    // 创建基础游戏信息
    auto& game = root.dict("game");
    game.set("name", "我的游戏");
    game.set("version", 1.0);
    game.set("active", true);

    // 创建国家数据
    auto& countries = game.dict("countries");
    
    // 添加一个示例国家
    auto country1 = std::make_shared<Dict>();
    country1->set("countryID", 1);
    country1->set("name", "星海狐狸帝国");
    country1->set("is_ai", false);
    country1->set("capitalPlanetID", 100);
    countries.set("1",country1);

    // 创建玩家数据
    auto& players = root.array("players");
    auto player1 = std::make_shared<Dict>();
    player1->set("name", "玩家1");
    player1->set("level", 10);
    players.push(player1);
    GameData::getInstance().setRoot(root);

    // 验证数据是否正确存储
    try {
        auto& gameData = GameData::getInstance();
        auto storedRoot = gameData.getRoot();
        
        // 验证游戏信息
        auto storedGame = storedRoot.get<std::shared_ptr<Dict>>("game");
        std::cout << "游戏名称: " << storedGame->get<std::string>("name") << std::endl;
        std::cout << storedGame->keys() << std::endl;
        auto a = Country::get_by_id("1");
        std::cout << "获取country1测试：" << a->name << std::endl;
    } catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }

    return 0;
}

