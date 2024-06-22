#include "GameWindow.h"
#include "raylib.h"
#include "Utils.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_set>

#define TICK_RATE_MS 16
#define MOVEMENT_SPEED 10

// Define an enum for input actions
enum InputAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    NONE
};
const float lerpThreshold = 0.2f;
const float accelerationFactor = 40.0f;
const float diagonalFactor = 0.5f;
// Define a struct to store input actions with their timestamps
struct TimedInput {
    InputAction action;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};

void GameWindow::setUsername(std::string x) {
    this->username = x;
}

// Global input buffer
std::unordered_set<InputAction> inputBuffer;

void GameWindow::runGame() {
    // Calculate the elapsed time since the last frame
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTime = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - gameTime).count();
    int ticksPassed = std::floor(elapsed / TICK_RATE_MS);

    // Update gameTime to the last processed tick time
    if (ticksPassed > 0) {
        gameTime += std::chrono::milliseconds(ticksPassed * TICK_RATE_MS);
    }

    // Capture and store input in the buffer
    captureInput();

    // Process input and update game logic for each tick
    for (int i = 0; i < ticksPassed; ++i) {
        processInput();
        updateGameLogic();
    }

    // Render the game
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(x, y, width, height, RED);
    DrawRectangle(newPlayerx, newPlayerY, width, height, RED);
    EndDrawing();
}

void GameWindow::captureInput() {
    // Capture multiple inputs simultaneously
    if (IsKeyDown(KEY_A) && username != "a") {
        inputBuffer.insert(MOVE_LEFT);
    }
    if (IsKeyDown(KEY_D) && username != "a") {
        if (!start) {
            start = true;
            testTime = std::chrono::steady_clock::now();
            std::cout << "test init" << "\n";
        }
        inputBuffer.insert(MOVE_RIGHT);
    }
    if (IsKeyDown(KEY_W) && username != "a") {
        inputBuffer.insert(MOVE_UP);
    }
    if (IsKeyDown(KEY_S) && username != "a") {
        inputBuffer.insert(MOVE_DOWN);
    }
    // Add other global inputs here
    if (Utils::IsKeyPressedGlobal(VK_UP) && username == "a") {
        inputBuffer.insert(MOVE_UP);
    }
    if (Utils::IsKeyPressedGlobal(VK_LEFT) && username == "a") {
        inputBuffer.insert(MOVE_LEFT);
    }
    if (Utils::IsKeyPressedGlobal(VK_DOWN) && username == "a") {
        inputBuffer.insert(MOVE_DOWN);
    }
    if (Utils::IsKeyPressedGlobal(VK_RIGHT) && username == "a") {
        inputBuffer.insert(MOVE_RIGHT);
    }
}


void GameWindow::processInput() {
    Vector2 movementDirection = {0,0};
    for (auto input : inputBuffer) {
        switch (input) {
            case MOVE_LEFT:
                x -= 1;
                break;
            case MOVE_RIGHT:
                x += 1;
                break;
            case MOVE_UP:
                y -= 1;
                break;
            case MOVE_DOWN:
                y += 1;
                break;
            case NONE:
                break;
        }
    }
    inputBuffer.clear();

}

void GameWindow::updateGameLogic() {
    // Example game logic update, you can add more logic here
    if (x > 1500 && pass == false) {
        pass = true;
        auto currentFrameTime = std::chrono::steady_clock::now();
        std::cout << "\n" << std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - testTime).count();
    }
}
