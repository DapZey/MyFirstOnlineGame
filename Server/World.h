//
// Created by benra on 6/26/2024.
//

#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H
#include "chrono"
#include "cmath"
#include "Utils.h"
#define TICK_RATE_MS 3
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
    float momentum;
    int score = 0;
    bool moving = false;
};
struct Border{
    int x;
    int y;
    int width;
    int height;
};
struct Line{
    Vector2 start = {0,0};
    Vector2 end = {0,0};
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
    std::vector<Line> lines;
public:
    bool needToUpdateScore = false;
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
