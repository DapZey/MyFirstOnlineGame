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
    DrawTexturePro(assets,{1753,489,902,1515},{0,0,SCREEN_HEIGHT,SCREEN_WIDTH},{0,0},0,WHITE);
    DrawTexturePro(assets,{3070,750,198,207},{x-RADIUS,y-RADIUS,RADIUS*2,RADIUS*2},{0,0},0,WHITE);
    DrawTexturePro(assets,{3070,750,198,207},{(float)newPlayerx-RADIUS,(float)newPlayerY-RADIUS,RADIUS*2,RADIUS*2},{0,0},0,WHITE);
    DrawTexturePro(assets,{2901,785,111,111},{(float)ballX-BALL_RADIUS,(float)ballY-BALL_RADIUS,BALL_RADIUS*2,BALL_RADIUS*2},{0,0},0,WHITE);
    world.draw();
    if (username == "a"){
        DrawTextPro(GetFontDefault(),TextFormat("FPS: %i", GetFPS()),{505,700},{playerFollowCamera.target.x,playerFollowCamera.target.y},180,20,5,GREEN);
        DrawTextPro(GetFontDefault(),TextFormat("Player 1 score: %i", player1Score),{505,680},{playerFollowCamera.target.x,playerFollowCamera.target.y},180,20,5,GREEN);
        DrawTextPro(GetFontDefault(),TextFormat("Player 2 score: %i", player2Score),{505,660},{playerFollowCamera.target.x,playerFollowCamera.target.y},180,20,5,GREEN);
    }
    else {
        DrawTextPro(GetFontDefault(),TextFormat("Player 1 score: %i", player1Score),{playerFollowCamera.target.x,playerFollowCamera.target.y},{495,680},0,20,5,GREEN);
        DrawTextPro(GetFontDefault(),TextFormat("Player 2 score: %i", player2Score),{playerFollowCamera.target.x,playerFollowCamera.target.y},{495,660},0,20,5,GREEN);
        DrawTextPro(GetFontDefault(),TextFormat("FPS: %i", GetFPS()),{playerFollowCamera.target.x,playerFollowCamera.target.y},{495,700},0,20,5,GREEN);
    }
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