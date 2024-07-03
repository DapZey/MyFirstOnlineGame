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
#include <iostream>
class Utils {
public:
    static bool IsKeyPressedGlobal(int key) {
        return GetAsyncKeyState(key) & 0x8000;
    }
    static bool CheckCollisionCircleLine(Vector2 circleCenter, float radius, Vector2 lineStart, Vector2 lineEnd)
    {
        // Vector from line start to circle center
        Vector2 lineToCircle = { circleCenter.x - lineStart.x, circleCenter.y - lineStart.y };
        // Vector representing the line segment
        Vector2 lineVector = { lineEnd.x - lineStart.x, lineEnd.y - lineStart.y };

        // Calculate the projection of the circle center onto the line
        float lineLengthSquared = lineVector.x * lineVector.x + lineVector.y * lineVector.y;
        float t = fmax(0, fmin(1, (lineToCircle.x * lineVector.x + lineToCircle.y * lineVector.y) / lineLengthSquared));

        // Find the closest point on the line segment to the circle center
        Vector2 closestPoint = { lineStart.x + t * lineVector.x, lineStart.y + t * lineVector.y };

        // Calculate the distance from the circle center to the closest point
        float distanceSquared = (circleCenter.x - closestPoint.x) * (circleCenter.x - closestPoint.x) +
                                (circleCenter.y - closestPoint.y) * (circleCenter.y - closestPoint.y);

        // Check if the distance is less than or equal to the circle's radius squared
        return distanceSquared <= radius * radius;
    }
    static std::string extractSubstringBetweenDelimiters(const std::string &str, char delimiter) {
        size_t startPos = str.find(delimiter);
        if (startPos == std::string::npos) {
            return "";  // Delimiter not found
        }

        size_t endPos = str.find(delimiter, startPos + 1);
        if (endPos == std::string::npos) {
            return "";  // Second occurrence of the delimiter not found
        }

        // Extract the substring between the delimiters
        return str.substr(startPos, endPos - startPos + 1);
    }

// Function to extract substrings between *, ?, and $ delimiters
    static std::vector<std::string> extractSubstrings(const std::string &str) {
        std::vector<std::string> result;
        std::string starSubstring = extractSubstringBetweenDelimiters(str, '*');
        if (!starSubstring.empty()) {
            result.push_back(starSubstring);
        }

        std::string questionSubstring = extractSubstringBetweenDelimiters(str, '?');
        if (!questionSubstring.empty()) {
            result.push_back(questionSubstring);
        }

        std::string dollarSubstring = extractSubstringBetweenDelimiters(str, '$');
        if (!dollarSubstring.empty()) {
            result.push_back(dollarSubstring);
        }
        return result;
    }
    static bool containsChar(const std::string &str, char ch) {
        // Use the find method to check if the character is in the string
        return str.find(ch) != std::string::npos;
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
