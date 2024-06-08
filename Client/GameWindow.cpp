//
// Created by benra on 6/4/2024.
//

#include "GameWindow.h"
#include "raylib.h"
#include "Utils.h"
void GameWindow::setUsername(std::string x) {
    this->username = x;
}
void GameWindow::runGame() {
    std::string s = " ";
        if (IsKeyDown(KEY_A) && username != "a") {
            x -= 1;
        }
        if (IsKeyDown(KEY_D) && username != "a") {
            x += 1;
        }
        if (IsKeyDown(KEY_W) && username != "a") {
            y -= 1;
        }
        if (IsKeyDown(KEY_S) && username != "a") {
            y += 1;
        }
        if (Utils::IsKeyPressedGlobal(VK_UP) && username == "a") {
            y -= 1;
        }
        if (Utils::IsKeyPressedGlobal(VK_LEFT) && username == "a") {
            x -= 1;
        }
        if (Utils::IsKeyPressedGlobal(VK_DOWN) && username == "a") {
            y+=1;
        }
        if (Utils::IsKeyPressedGlobal(VK_RIGHT) && username == "a") {
            x += 1;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(x, y, width, height, RED);
        DrawRectangle(newPlayerx, newPlayerY, width, height, RED);
        EndDrawing();
}