#include "GameWindow.h"
#include "raylib.h"
#include "Utils.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_set>

#define TICK_RATE_MS 16
#define MOVEMENT_SPEED 13
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define PLAYER_OFFSET 600
#define RADIUS 30
#define CAMERA_LERP_RATE 10
#define BALL_RADIUS 15

// Define an enum for input actions
GameWindow::GameWindow() {
    playerFollowCamera.target={float(SCREEN_WIDTH)/ 2,float (SCREEN_HEIGHT) / 2};
    playerFollowCamera.offset = (Vector2){ float(SCREEN_WIDTH)/ 2, float (SCREEN_HEIGHT + PLAYER_OFFSET) / 2 };
    playerFollowCamera.rotation = 0.0f;
    playerFollowCamera.zoom = 1.0f;
}
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
struct TimedInput {
    InputAction action;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};

void GameWindow::setUsername(std::string text) {
    this->username = text;
    if (username == "a"){
        playerFollowCamera.rotation = 180.0f;
        x = 100;
        y = 200;
        newPlayerY = 660;
        newPlayerx = 220;
    }
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
    BeginMode2D(playerFollowCamera);
    DrawCircle(x, y, RADIUS, RED);
    DrawCircle(newPlayerx, newPlayerY, RADIUS, RED);
    DrawCircle(ballX, ballY, BALL_RADIUS, BLUE);
    RaylibDrawText(TextFormat("Pos Y: %f", y), 10, 10, 20, LIGHTGRAY);
    RaylibDrawText(TextFormat("FPS: %i", GetFPS()), 10, 40, 20, LIGHTGRAY);
    RaylibDrawText(TextFormat("Pos X: %f", x), 10, 70, 20, LIGHTGRAY);
    world.draw();
    EndDrawing();
    EndMode2D();
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
        inputBuffer.insert(MOVE_DOWN);
    }
    if (Utils::IsKeyPressedGlobal(VK_LEFT) && username == "a") {
        inputBuffer.insert(MOVE_RIGHT);
    }
    if (Utils::IsKeyPressedGlobal(VK_DOWN) && username == "a") {
        inputBuffer.insert(MOVE_UP);
    }
    if (Utils::IsKeyPressedGlobal(VK_RIGHT) && username == "a") {
        inputBuffer.insert(MOVE_LEFT);
    }
}

const float gameTickrate = 0.016;
void GameWindow::processInput() {
    Vector2 movementDirection = {0.0,0.0};
    for (auto input : inputBuffer) {
        switch (input) {
            case MOVE_LEFT:
                movementDirection.x -= 1;
//                x -= MOVEMENT_SPEED;
                break;
            case MOVE_RIGHT:
                movementDirection.x += 1;
//                x += MOVEMENT_SPEED;
                break;
            case MOVE_UP:
                movementDirection.y -= 1;
//                y -= MOVEMENT_SPEED;
                break;
            case MOVE_DOWN:
                movementDirection.y += 1;
//                y += MOVEMENT_SPEED;
                break;
            case NONE:
                break;
        }
    }
    inputBuffer.clear();
    //normalize for diags speed
    movementDirection = Utils::normalize(movementDirection);
    //smooth changes in direction for diagonals
    float lerpFactor = accelerationFactor * gameTickrate;
    if (direction.x != 0 && direction.y != 0 &&
        (movementDirection.x == 0 || movementDirection.y == 0)) {
        lerpFactor *= diagonalFactor;
    }
    if (movementDirection.x != 0 || movementDirection.y != 0) {
        direction = Utils::Vector2Lerp(direction, movementDirection, lerpFactor);
    } else {
        direction = Utils::Vector2Lerp(direction, {0, 0}, lerpFactor);
    }
    Vector2 pos = world.findCollision({x,y},{x + direction.x * MOVEMENT_SPEED, y + direction.y * MOVEMENT_SPEED},RADIUS);
x = pos.x;
y = pos.y;
    if (Utils::Vector2Distance(direction, movementDirection) < lerpThreshold) {
        direction = movementDirection; // If the difference is small, set directly
    }
    playerFollowCamera.target = Utils::Vector2Lerp(playerFollowCamera.target, {x,y}, CAMERA_LERP_RATE * gameTickrate);
    if (Utils::Vector2Distance(playerFollowCamera.target, {x,y}) < lerpThreshold) {
        playerFollowCamera.target = {x,y};
    }
}

void GameWindow::updateGameLogic() {
    if (x > 1500 && pass == false) {
        pass = true;
        auto currentFrameTime = std::chrono::steady_clock::now();
        std::cout << "\n" << std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - testTime).count();
    }
}