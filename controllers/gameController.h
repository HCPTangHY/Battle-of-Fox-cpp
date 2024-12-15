#pragma once

#include "BaseController.h"

class GameController : public BaseController {
private:
    GameModel* model;
    
public:
    GameController(GameManager* manager);
    
    void init() override;
    void update(float delta) override;
    
    // 游戏相关控制方法
    void handle_input(const Input& input);
    void process_game_logic();
};
