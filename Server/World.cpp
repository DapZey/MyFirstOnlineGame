#include "World.h"
World::World() {
    borders.push_back({0, 0, 800, 10});    // Top border
    borders.push_back({0, 990, 800, 10});  // Bottom border
    borders.push_back({0, 0, 10, 1000});   // Left border
    borders.push_back({790, 0, 10, 1000}); // Right border

    // Initialize the ball with some default values
    ball.x = 400;
    ball.y = 500;
    gameTime = std::chrono::steady_clock::now();
}

Vector2 World::findCollision(const Vector2 current, const Vector2 expected, int radius) {
    Vector2 collisionPoint = {current.x, current.y};
    std::vector<float> positionsX = Utils::generatePositions(current.x, expected.x);
    std::vector<float> positionsY = Utils::generatePositions(current.y, expected.y);

    for (size_t i = 0; i < positionsX.size(); ++i) {
        bool collisionDetected = false;
        Ball tempBall = ball;
        tempBall.x = positionsX[i];
        for (const auto& border : borders) {
            if (CheckCollisionCircleRec({tempBall.x,tempBall.y},15,border)) {
                collisionDetected = true;
                collisionType = wall;
                break;
            }
            for (int k = 0; k < 2; k++){
                if (Utils::CheckCollisionCircles({tempBall.x, tempBall.y},15,{players[k].x,players[k].y},30)){
                    collisionDetected = true;
                    collisionType = playerStationary;
                    break;
                }
            }
        }
        if (!collisionDetected) {
            collisionPoint.x = positionsX[i];
        } else {
            break;
        }
    }

    for (size_t i = 0; i < positionsY.size(); ++i) {
        bool collisionDetected = false;
        Ball tempBall = ball;
        tempBall.y = positionsY[i];
        for (const auto& border : borders) {
            if (CheckCollisionCircleRec({tempBall.x,tempBall.y},15,border)) {
                collisionDetected = true;
                collisionType = wall;
                break;
            }
        }
        for (int k = 0; k < 2; k++){
            if (Utils::CheckCollisionCircles({tempBall.x, tempBall.y},15,{players[k].x,players[k].y},30)){
                collisionDetected = true;
                collisionType = playerStationary;
                break;
            }
        }
        if (!collisionDetected) {
            collisionPoint.y = positionsY[i];
        } else {
            break;
        }
    }

    return collisionPoint;
}

bool World::CheckCollisionCircleRec(const Vector2 center, float radius, const Border& border) {
    bool collision = false;

    float recCenterX = border.x + border.width / 2.0f;
    float recCenterY = border.y + border.height / 2.0f;

    float dx = std::fabs(center.x - recCenterX);
    float dy = std::fabs(center.y - recCenterY);

    if (dx > (border.width / 2.0f + radius)) { return false; }
    if (dy > (border.height / 2.0f + radius)) { return false; }

    if (dx <= (border.width / 2.0f)) { return true; }
    if (dy <= (border.height / 2.0f)) { return true; }

    float cornerDistanceSq = (dx - border.width / 2.0f) * (dx - border.width / 2.0f) +
                             (dy - border.height / 2.0f) * (dy - border.height / 2.0f);

    collision = (cornerDistanceSq <= (radius * radius));

    return collision;
}

void World::run() {
    auto currentFrameTime = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - gameTime).count();
    int ticksPassed = std::floor(elapsed / TICK_RATE_MS);
    if (ticksPassed > 0) {
        gameTime += std::chrono::milliseconds(ticksPassed * TICK_RATE_MS);
    }
    for (int i = 0; i < ticksPassed; i++) {
        moveBall();
    }
}

void World::moveBall() {
    // Decrease momentum before movement calculation (friction)
    ball.momentum -= 0.1;
    if (ball.momentum <= 0) {
        ball.momentum = 0;  // Ensure momentum doesn't go negative
    }
    Vector2 directionCurrent = Utils::normalize({ball.direction.x,ball.direction.y});
    float newXPos = ball.x + (directionCurrent.x * ball.momentum);
    float newYPos = ball.y + (directionCurrent.y * ball.momentum);
    Vector2 newPos = findCollision({ball.x, ball.y}, {newXPos, newYPos}, ball.radius);
    ball.x = newPos.x;
    ball.y = newPos.y;
    if (newXPos != ball.x) {
        if (collisionType == wall){
            ball.direction.x = -ball.direction.x;
        }
        if (collisionType == playerStationary){

        }
        ball.momentum -= 0.5;
        if (ball.momentum <= 0) {
            ball.momentum = 0;  // Ensure momentum doesn't go negative
        }
    }

    if (newYPos != ball.y) {
        if (collisionType == wall){
            ball.direction.y = -ball.direction.y;
        }
        if (collisionType == playerStationary){

        }
        ball.momentum -= 0.5;
        if (ball.momentum <= 0) {
            ball.momentum = 0;  // Ensure momentum doesn't go negative
        }
    }
    if (ball.momentum == 0){
        ball.momentum = 10;
    }
}