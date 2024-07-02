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
    float x;
    float y;
    Vector2 direction = {1,1};
    float momentum = 0;
    float radius = 15;
};
struct Player{
    bool connected = false;
    float x = 0;
    float xPrev = 0;
    float y = 0;
    float yPrev = 0;
    int radius = 30;
    Vector2 direction = {0,0};
    Vector2 directionPrev = {0,0};
    Vector2 toMove = {0,0};
    int momentum;
};
struct Border{
    int x;
    int y;
    int width;
    int height;
};
enum CollisionType{
    wall,
    playerStationary,
    playerMoving
};
class World {
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> gameTime = std::chrono::steady_clock::now();
    int elapsed = 0;
    bool CheckCollisionCircleRec(const Vector2 center, float radius, const Border& border);
    Vector2 findCollision(Vector2 current, Vector2 expected, int radius);
    std::vector<Border> borders;
    CollisionType collisionType = wall;
public:
    Player player1;
    Player player2;
    Player players[2] = {player1,player2};
    Ball ball;
    void run();
    void moveBall();
    World();
    void movePlayer();
};


#endif //SERVER_WORLD_H
