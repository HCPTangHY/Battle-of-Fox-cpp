#include "util/Container.h"
#include "util/GameData.h"
#include "models/Country.h"
#include "controllers/MapGenerater.h"
#include <iostream>
// #include "util/Voronoi.h"

namespace BoF {
int main() {
    system("chcp 65001");
    Dict root;  // 创建根字典

    // 创建基础游戏信息
    auto& game = root["game"];
    game["name"] = "我的游戏";
    game["version"] = 1.0;
    game["active"] = true;

    // 创建国家数据
    auto& countries = game["countries"];
    
    // 添加一个示例国家
    auto country1 = json::make_dict();
    country1["countryID"] = 1;
    country1["name"] = "星海狐狸";
    country1["is_ai"] = false;

    GameData::getInstance().setRoot(root);

    BoF::Dict grid = BoF::MapGenerater::generate_grid();
    Dict cells = grid["cells"];
    cells["h"] = BoF::MapGenerater::heightmap_generate(grid,1000,1000);
    grid["cells"] = cells;
    Array paths = BoF::MapGenerater::heightmap_paths_calculate(grid);
    std::cout << "高度图: " << paths.size() << std::endl;

    return 0;
}
}
int main() {
    return BoF::main();
}