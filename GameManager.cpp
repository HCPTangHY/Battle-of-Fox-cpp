// GameManager.cpp
#include "GameManager.h"
#include <iostream>

GameManager* GameManager::instance = nullptr;

GameManager* GameManager::getInstance() {
    if (instance == nullptr) {
        instance = new GameManager();
    }
    return instance;
}

GameManager::GameManager() : 
    isRunning(false),
    isPaused(false),
    gameSpeed(1.0),
    currentTick(0.0)
{
}

bool GameManager::initialize() {
    // 初始化各个系统
    return true;
}

void GameManager::cleanup() {
    // 清理资源
}

void GameManager::startGame() {
    isRunning = true;
    isPaused = false;
}

void GameManager::pauseGame() {
    isPaused = true;
}

void GameManager::resumeGame() {
    isPaused = false;
}

void GameManager::endGame() {
    isRunning = false;
}

void GameManager::setGameSpeed(double speed) {
    gameSpeed = speed;
}

double GameManager::getGameSpeed() const {
    return gameSpeed;
}

double GameManager::getCurrentTick() const {
    return currentTick;
}

void GameManager::update(double deltaTime) {
    if (!isRunning || isPaused) {
        return;
    }
    
    // 更新游戏tick
    currentTick += deltaTime * gameSpeed;
    
    // 调用各系统的更新 (后续实现)
    // economySystem->update(deltaTime);
    // combatSystem->update(deltaTime);
    // etc...
}

int main() {
    // 获取实例
    GameManager* game = GameManager::getInstance();
    
    // 初始化
    game->initialize();
    
    // 测试输出
    std::cout << "Hello World from GameManager!" << std::endl;
    std::cout << "Current game speed: " << game->getGameSpeed() << std::endl;
    
    // 清理
    game->cleanup();
    
    return 0;
}