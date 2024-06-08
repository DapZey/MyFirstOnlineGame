//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_CONNECTWINDOW_H
#define CLIENT_CONNECTWINDOW_H
#include "raylib.h"
#define MAX_INPUT_CHARS 28

class ConnectWindow {
    int screenWidth;
    RaylibRectangle textBox;// NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;
    bool mouseOnText = false;
public:
    char name[MAX_INPUT_CHARS + 1] = "\0";
    ConnectWindow(int screenWidth);
    int ShowPage();
};


#endif //CLIENT_CONNECTWINDOW_H
