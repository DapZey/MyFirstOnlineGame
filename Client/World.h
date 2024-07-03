//
// Created by benra on 6/25/2024.
//

#ifndef CLIENT_WORLD_H
#define CLIENT_WORLD_H

#include "raylib.h"
#include "vector"
class World {
    struct Line{
        Vector2 start = {0,0};
        Vector2 end = {0,0};
    };
private:
    std::vector<RaylibRectangle> borders;
    std::vector<Line> lines;
public:
    void draw();
    World();
    std::vector<float> generatePositions(float currentPos, float expectedPos);
    Vector2 findCollision(Vector2 current, Vector2 expected, int radius);
    bool checkCollisionGeneral(Vector2 current, int radius);
};


#endif //CLIENT_WORLD_H
