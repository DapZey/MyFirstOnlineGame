//
// Created by benra on 6/4/2024.
//

#include "ConnectWindow.h"
#include <vector>
#include <string>
#include "Utils.h"

ConnectWindow::ConnectWindow(int screenWidth) {
    this->screenWidth = screenWidth;
    this->textBox = { screenWidth/8.0f, 180, (float)(screenWidth/8.0 * 6), 50 };
}

int ConnectWindow::ShowPage() {
    if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
    else mouseOnText = false;

    if (mouseOnText)
    {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
            {
                name[letterCount] = (char)key;
                name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }
            key = GetCharPressed();  // Check next character in the queue
        }
        if (IsKeyDown(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        }
    }
    else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    RaylibDrawText("Type:{ip}/{port}/{username} in the textbox and press enter", screenWidth/8.0, 140, 20, GRAY);
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
    else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
    RaylibDrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
    RaylibDrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);
    EndDrawing();
    if (IsKeyPressed(KEY_ENTER)){
        return 1;
    }
    return 0;
}