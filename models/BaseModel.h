#include "../util/Container.h"
#include "../util/GameData.h"

namespace BoF {
class BaseModel {

public:
    BaseModel() = default;
    ~BaseModel() = default;

    static Dict& get_root() {
        return GameData::getInstance().getRoot();
    }
    static std::shared_ptr<Dict> get_game_dict() {
        return BaseModel::get_root()["game"];
    }
    void update() {};
private:
    std::shared_ptr<Dict> target_dict;
};

}