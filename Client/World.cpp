//
// Created by benra on 6/25/2024.
//

#include "World.h"
#include "cmath"
#include <iostream>
World::World() {
    borders.push_back({ -100, -100, 600, 10 });  // Top border
    borders.push_back({ -100, 800, 600, 10 });   // Bottom border
    borders.push_back({ -100, -100, 10, 900});  // Left border
    borders.push_back({ 500, -100, 10, 900 });   // Right border

}
void World::draw() {
    for (int i = 0; i < borders.size(); i++){
        DrawRectangleRec(borders[i],BROWN);
    }
}

bool World::checkCollisionGeneral(Vector2 current, int radius) {
    for (int i = 0; i < borders.size(); i++){
        if (CheckCollisionCircleRec({ current.x, current.y }, radius, borders[i])) {
            return true;
        }
    }
    return false;
}
Vector2 World::findCollision(Vector2 current, Vector2 expected, int radius) {
    Vector2 collisionPoint = { current.x, current.y };
    std::vector<float> positionsX = generatePositions(current.x, expected.x);
    std::vector<float> positionsY = generatePositions(current.y, expected.y);
    for (size_t i = 0; i < positionsX.size(); ++i) {
        bool collisionDetected = false;
        for (size_t j = 0; j < borders.size(); ++j) {
            if (CheckCollisionCircleRec({ positionsX[i], current.y }, radius, borders[j])) {
                collisionDetected = true;
                break;
            }
        }
        if (!collisionDetected) {
            collisionPoint.x = positionsX[i];
        }
        else {
            break;
        }
    }
    for (size_t i = 0; i < positionsY.size(); ++i) {
        bool collisionDetected = false;
        for (size_t j = 0; j < borders.size(); ++j) {
            if (CheckCollisionCircleRec({ current.x, positionsY[i] }, radius, borders[j])) {
                collisionDetected = true;
                break;
            }
        }
        if (!collisionDetected) {
            collisionPoint.y = positionsY[i];
        }
        else {
            break;
        }
    }
    return collisionPoint;
}

std::vector<float> World::generatePositions(float currentPos, float expectedPos) {
    std::vector<float> positions;
    float dx = expectedPos - currentPos;
    float distance = std::abs(dx);
    float stepSize = 0.1f;
    int numSteps = static_cast<int>(distance / stepSize);
    positions.push_back(currentPos);
    for (int i = 1; i <= numSteps; ++i) {
        float t = static_cast<float>(i) / numSteps;
        float x = currentPos + t * dx;
        positions.push_back(x);
    }
    return positions;
}