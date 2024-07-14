#include "World.h"
World::World() {
    borders.push_back({0, 0, 800, 10});    // Top border
    borders.push_back({0, 990, 800, 10});  // Bottom border
    borders.push_back({0, 0, 10, 1000});   // Left border
    borders.push_back({790, 0, 10, 1000}); // Right border
    lines.push_back({0,100,100,0});
    lines.push_back({0,900,100,1000});
    lines.push_back({700,0,800,100});
    lines.push_back({700,1000,800,900});
    // Initialize the ball with some default values
    ball.x = 387;
    ball.y = 505;
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
        }
        for (int j = 0; j < lines.size();++j){
            if (Utils::CheckCollisionCircleLine({tempBall.x,tempBall.y}, 15,lines[j].start,lines[j].end)){
                collisionDetected = true;
                collisionType = wall;
                ball.momentum += 1;
                if (ball.momentum > 5){
                    ball.momentum = 5;
                }
                break;}
        }
        if (!collisionDetected|| collisionType == playerStationary) {
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
                if (tempBall.x > 345 && tempBall.x < 435){
                    if (tempBall.y > 500){
                        players[0].score++;
                    }
                    else {
                        players[1].score++;
                    }
                    needToUpdateScore = true;
                    ball.momentum = 0;
                    ball.direction = {0,0};
                    return {387,505};
                }
                break;
            }
        }
        for (int j = 0; j < lines.size();++j){
            if (Utils::CheckCollisionCircleLine({collisionPoint.x,tempBall.y}, 15,{lines[j].start},lines[j].end)){
                collisionDetected = true;
                collisionType = wall;
                ball.momentum +=1;
                if (ball.momentum > 5){
                    ball.momentum = 5;
                }
                break;
            }
        }
        if (!collisionDetected || collisionType == playerStationary) {
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
        movePlayer();
        moveBall();
    }
}

void World::movePlayer() {
    for (int i = 0; i < 2; i++){
        if (players[i].moving){
            players[i].momentum+=0.25;
            if (players[i].momentum > 5){
                players[i].momentum = 5;
            }
            players[i].moving = false;
        }
        players[i].momentum -= 0.025;
        if (players[i].momentum < 0){
            players[i].momentum = 0;
        }
        players[i].xPrev = players[i].x;
        players[i].yPrev = players[i].y;
        players[i].x = players[i].toMove.x;
        players[i].y = players[i].toMove.y;
        players[i].direction = Utils::normalize({players[i].x - players[i].xPrev, players[i].y - players[i].yPrev});
        Vector2 playerCollision = Utils::CheckCollisionCircles({ball.x,ball.y},15,{players[i].x,players[i].y},30);
        if (players[i].momentum > 0){
            collisionType = playerMoving;
        }
        if (playerCollision.x != 0 || playerCollision.y != 0){
            Vector2 directionFromPlayer = Utils::normalize({ball.x - players[i].x, ball.y - players[i].y});
            float totalRadius = 30 + 15;
            ball.x = players[i].x + directionFromPlayer.x * totalRadius;
            ball.y = players[i].y + directionFromPlayer.y * totalRadius;
            ball.direction = Utils::CombineVectors(Utils::returnDirectionVector({players[i].x,players[i].y},playerCollision),players[i].direction);
            if (collisionType == playerMoving){
                if (players[i].momentum > ball.momentum){
                    ball.momentum = players[i].momentum;
                }
                std::cout<<"momentum: "<<players[i].momentum<<"\n";
            }
        }
    }
}

void World::moveBall() {
    // Decrease momentum before movement calculation (friction)
    ball.momentum -= 0.0025;
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
    }

    if (newYPos != ball.y) {
        if (collisionType == wall){
            ball.direction.y = -ball.direction.y;
        }
    }
}