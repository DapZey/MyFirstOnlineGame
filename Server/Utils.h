//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "cmath"
#include <iostream>

struct Vector2{
    float x;
    float y;
};

class Utils {
public:
    static std::vector<float> generatePositions(float currentPos, float expectedPos) {
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
        positions.push_back(expectedPos);
        return positions;
    }
    static bool IsKeyPressedGlobal(int key) {
        return GetAsyncKeyState(key) & 0x8000;
    }
    static Vector2 returnDirectionVector(Vector2 center, Vector2 collisionPoint) {
        // Calculate the direction vector from center to collision point
        Vector2 direction = {0,0};
        direction.x = collisionPoint.x - center.x;
        direction.y = collisionPoint.y - center.y;

        // Calculate the length of the direction vector
        float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // Normalize the direction vector so its components are between -1 and 1
        if (length != 0) {
            direction.x /= length;
            direction.y /= length;
        }
        return direction;
    }
    static bool checkEqualVectors(Vector2 first, Vector2 second){
        if (first.x == second.x && first.y == second.y){
            return true;
        }
        return false;
    }
    static Vector2 CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
    {
        Vector2 toReturn = {0,0};
        float dx = center2.x - center1.x;      // X distance between centers
        float dy = center2.y - center1.y;      // Y distance between centers

        float distance = sqrtf(dx * dx + dy * dy); // Distance between centers

        if (distance <= (radius1 + radius2))
        {
            // Calculate the collision point
            float a = (radius1 * radius1 - radius2 * radius2 + distance * distance) / (2 * distance);
            float h = sqrtf(radius1 * radius1 - a * a);
            float cx2 = center1.x + a * (center2.x - center1.x) / distance;
            float cy2 = center1.y + a * (center2.y - center1.y) / distance;
            float collisionX = cx2 + h * (center2.y - center1.y) / distance;
            float collisionY = cy2 - h * (center2.x - center1.x) / distance;
            toReturn.x = collisionX;
            toReturn.y = collisionY;
        }
        return toReturn;
    }
    static std::vector<std::string> splitstringbychar(const std::string& input, const std::string& delimiters) {
        std::vector<std::string> result;
        std::string::size_type start = 0;
        std::string::size_type pos;

        // Iterate until there are no more delimiters found
        while ((pos = input.find_first_of(delimiters, start)) != std::string::npos) {
            // Add the substring before the delimiter
            result.push_back(input.substr(start, pos - start));
            // Update starting position to skip the delimiter
            start = pos + 1;
        }

        // Add the last substring after the last delimiter (or the entire string if no delimiters)
        result.push_back(input.substr(start));

        return result;
    }
    static Vector2 CombineVectors(Vector2 vec1, Vector2 vec2) {
        Vector2 combined;
        combined.x = vec1.x + vec2.x;
        combined.y = vec1.y + vec2.y;
        return normalize(combined);
    }
    static Vector2 normalize(const Vector2 &vec) {
        float mag = magnitude(vec);
        // Check if the magnitude is not zero to avoid division by zero
        if (mag != 0.0f) {
            return {vec.x / mag, vec.y / mag};
        } else {
            // If the magnitude is zero, return a zero vector or handle it as appropriate for your use case
            return {0.0f, 0.0f};
        }
    }
    static float magnitude(const Vector2& vec) {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }
    static Vector2 Vector2Lerp(const Vector2& a, const Vector2& b, float t) {
        return {
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t
        };
    }
    static float Vector2Distance(const Vector2& a, const Vector2& b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return sqrt(dx * dx + dy * dy);
    }
};


#endif //CLIENT_UTILS_H