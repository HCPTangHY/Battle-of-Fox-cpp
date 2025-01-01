#pragma once
#include "Container.h"

class GameData {
private:
    Dict root;
    GameData() = default;
    
public:
    static GameData& getInstance();

    GameData(const GameData&) = delete;

    GameData& operator=(const GameData&) = delete;

    void setRoot(const Dict& data) {
        root = data;
    }
    
    Dict& getRoot() {
        return root;
    }
};
