#include "GameData.h"

GameData& GameData::getInstance() {
    static GameData instance;
    return instance;
}