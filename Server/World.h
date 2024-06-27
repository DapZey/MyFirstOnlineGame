//
// Created by benra on 6/26/2024.
//

#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H
#include "chrono"
#include "cmath"
#include "Utils.h"
#define TICK_RATE_MS 16
struct Ball{
    float x = 400;
    float y = 500;
    Vector2 direction = {1,0};
    float momentum = 10;
};
struct Border{
    int x;
    int y;
    int width;
    int height;
};
class World {
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> gameTime = std::chrono::steady_clock::now();
    int elapsed = 0;
public:
    Ball ball;
    void run();
    void moveBall();
};


#endif //SERVER_WORLD_H
