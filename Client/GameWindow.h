//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_GAMEWINDOW_H
#define CLIENT_GAMEWINDOW_H

#include <string>
class GameWindow {
    int width = 50;
    int height = 50;
    std::string username = "";
public:
    int newPlayerx = 0;
    int newPlayerY = 0;
    int x = 0;
    int y = 0;
    void setUsername(std::string x);
    void runGame();
};


#endif //CLIENT_GAMEWINDOW_H
