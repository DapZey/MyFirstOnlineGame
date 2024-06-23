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
#include "raylib.h"
class Utils {
public:
    static bool IsKeyPressedGlobal(int key) {
        return GetAsyncKeyState(key) & 0x8000;
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
