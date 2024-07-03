//
// Created by benra on 6/25/2024.
//

#include "World.h"
#include "cmath"
#include "Utils.h"
#include <iostream>
World::World() {
    borders.push_back({ 0, 0, 800, 10 });  // Top border
    borders.push_back({ 0, 1000, 800, 10 });   // Bottom border
    borders.push_back({ 0, 0, 10, 1000});  // Left border
    borders.push_back({ 800, 0, 10, 1000 });   // Right border
    borders.push_back({0,500,800,10});
    lines.push_back({0,100,100,0});
    lines.push_back({0,900,100,1000});
    lines.push_back({700,0,800,100});
    lines.push_back({700,1000,800,900});
}
void World::draw() {
    for (int i = 0; i < borders.size(); i++){
        DrawRectangleRec(borders[i],BROWN);
    }
    for (int i =0; i < lines.size(); i++){
        DrawLine(lines[i].start.x,lines[i].start.y,lines[i].end.x,lines[i].end.y,BROWN);
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
        for (int j = 0; j < lines.size();++j){
            if (Utils::CheckCollisionCircleLine({positionsX[i], current.y }, radius,{lines[j].start},lines[j].end)){
            collisionDetected = true;
            break;}
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
        for (int j = 0; j < lines.size();++j){
            if (Utils::CheckCollisionCircleLine({collisionPoint.x,positionsY[i] }, radius,{lines[j].start},lines[j].end)){
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