#pragma once

#include "../GameManager.h"
#include "../forward_declarations.h"

class BaseController {
protected:
    GameManager* game_manager;
    
public:
    BaseController(GameManager* manager) : game_manager(manager) {}
    virtual ~BaseController() = default;
    
    virtual void init() = 0;
    virtual void update(float delta) = 0;
};
