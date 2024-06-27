//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_GAMEWINDOW_H
#define CLIENT_GAMEWINDOW_H

#include <string>
#include <chrono>
#include "raylib.h"
#include "World.h"
class GameWindow {
    std::chrono::time_point<std::chrono::steady_clock> gameTime = std::chrono::steady_clock::now();
    int tickCount = 0;
    int width = 50;
    int height = 50;
    std::string username = "";
    bool pass = false;
    bool start = false;
    std::chrono::time_point<std::chrono::steady_clock> testTime;
    void processInput();
    void updateGameLogic();
    void captureInput();
    Camera2D playerFollowCamera = {0};
public:
    World world;
    GameWindow();
    int newPlayerx = 100;
    int newPlayerY = 200;
    Vector2 playerPos = {0,0};
    Vector2 direction = {0,0};
    float x = 220;
    float y = 660;
    int ballX = 400;
    int ballY = 500;
    void setUsername(std::string x);
    void runGame();
};


#endif //CLIENT_GAMEWINDOW_H