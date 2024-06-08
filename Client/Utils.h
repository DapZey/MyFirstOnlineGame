//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
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
};


#endif //CLIENT_UTILS_H
