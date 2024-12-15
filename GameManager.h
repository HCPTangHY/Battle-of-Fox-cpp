// GameManager.h
#pragma once

class GameManager {
private:
    // 单例相关
    static GameManager* instance;
    GameManager(); // 私有构造函数
    
    // 游戏状态
    bool isRunning;
    bool isPaused;
    double gameSpeed;
    double currentTick;
    
    // 各模块指针 (后续添加)
    // EconomySystem* economySystem;
    // CombatSystem* combatSystem;
    // etc...

public:
    // 单例访问
    static GameManager* getInstance();
    
    // 初始化与清理
    bool initialize();
    void cleanup();
    
    // 游戏循环控制
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame();
    
    // 时间控制
    void setGameSpeed(double speed);
    double getGameSpeed() const;
    double getCurrentTick() const;
    
    // 主循环更新
    void update(double deltaTime);
    
    // 前端通信接口 (后续实现)
    // void handleGodotCommand();
    // void syncDataWithGodot();
    
    // 禁用拷贝
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
};
