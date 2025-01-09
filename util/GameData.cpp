#include "GameData.h"

namespace BoF {
GameData& GameData::getInstance() {
    static GameData instance;
    return instance;
}
}