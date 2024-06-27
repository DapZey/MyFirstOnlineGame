//
// Created by benra on 6/26/2024.
//

#include "World.h"
#include <iostream>
void World::run() {
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTime = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - gameTime).count();
    int ticksPassed = std::floor(elapsed / TICK_RATE_MS);
    if (ticksPassed > 0) {
        gameTime += std::chrono::milliseconds(ticksPassed * TICK_RATE_MS);
    }
    for (int i = 0; i < ticksPassed; i++){
        moveBall();
    }
}
void World::moveBall() {
    ball.x += (ball.direction.x * ball.momentum);
    ball.y += (ball.direction.y * ball.momentum);
    if (ball.momentum > 0){
        ball.momentum-= 0.1;
    }
    else{
        ball.momentum = 10;
        ball.x = 400;
        ball.y = 500;
    }
}