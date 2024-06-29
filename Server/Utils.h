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
    static bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
    {
        bool collision = false;

        float dx = center2.x - center1.x;      // X distance between centers
        float dy = center2.y - center1.y;      // Y distance between centers

        float distance = sqrtf(dx*dx + dy*dy); // Distance between centers

        if (distance <= (radius1 + radius2)) collision = true;

        return collision;
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