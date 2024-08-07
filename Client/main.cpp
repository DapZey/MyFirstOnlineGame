#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <chrono>
#include "raylib.h"
#include <vector>
#include "ConnectWindow.h"
#include "Utils.h"
#include "Network.h"
#include "GameWindow.h"
#include "World.h"
#define MAX_INPUT_CHARS 28


int main(int argc, char* argv[]) {
    const int screenWidth = 1000;
    const int screenHeight = 800;
    const std::string responseToRtt = "&";
    const std::string myRTT = "%";
    bool rttCheck = true;
    InitWindow(screenWidth, screenHeight, "Player Class Example");
    SetTargetFPS(1000);
    ConnectWindow connectionPage(screenWidth);
    int foundConnection = 0;
    Network network;
    network.startup();
    GameWindow gameWindow;
    std::chrono::time_point<std::chrono::steady_clock> lastSendRecvTimeGeneral = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastSendRecvTimeRTT = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> RTTGap = std::chrono::steady_clock::now();
    int updateFreq = 0;
    int rttCount = 0;
    int currentRTT = 0;
    int averageRTT = 0;
    long long totalRTT = 0;
    float oldX = 0;
    float oldY = 0;
    std::string data;
    bool needToRespond = false;
    bool needToSendCoords = false;
    while (!WindowShouldClose()) {
        if (foundConnection ==0){
            foundConnection = connectionPage.ShowPage();
        }
        else {
            gameWindow.runGame();
            auto now = std::chrono::steady_clock::now();
            auto elapsedGeneral = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendRecvTimeGeneral);
            auto elapsedRTT = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendRecvTimeRTT);
            std::string stringToSend;
            if (!data.empty()){
                std::cout<<data<<"\n";
            }
            if (Utils::containsChar(data,'&')){
                needToRespond = true;
            }
            if (Utils::containsChar(data,'%')){
                auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now - RTTGap);
                currentRTT = time.count();
                rttCount++;
                totalRTT += currentRTT;
                averageRTT = (int)(totalRTT / rttCount);
                updateFreq = (int)(((0.9 * currentRTT + 0.1 * averageRTT) - averageRTT)/2);
                std::cout<<"a"<<updateFreq<<"\n";
                rttCheck = true;
                if (updateFreq < 0){
                    updateFreq = 0;
                    totalRTT = 0;
                    rttCount = 0;
                    std::cout<<"reset update freq\n";
                }
            }
            if (elapsedRTT.count() >= 1000 && rttCheck) {
                std::cout<<"sent"<<"\n";
                stringToSend += myRTT;
                lastSendRecvTimeRTT = now;
                RTTGap = now;
                rttCheck = false;
            }
            if (IsKeyDown(KEY_R)){
                stringToSend += "@";
            }
            if (IsKeyDown(KEY_P)){
                gameWindow.x = 100;
                gameWindow.y = 100;
            }
            if (needToRespond){
                stringToSend += responseToRtt;
                needToRespond = false;
            }
            std::string coordinates = "*"+std::to_string((int)gameWindow.x) + "," + std::to_string((int)gameWindow.y);
            if (oldX != gameWindow.x || oldY != gameWindow.y){
                needToSendCoords = true;
            }
            if (needToSendCoords){
                if ((elapsedGeneral.count() >= updateFreq + 1)) {
                    stringToSend += coordinates;
                    lastSendRecvTimeGeneral = now;
                    oldX = gameWindow.x;
                    oldY = gameWindow.y;
                    needToSendCoords = false;
                } else {
                    std::cout<<elapsedGeneral.count()<<"skipped"<<"\n";
                }
            }
            std::vector<std::string> extractSubstrings = Utils::extractSubstrings(data);
            for (int i = 0; i < extractSubstrings.size(); i++){
                if (extractSubstrings[i][0] == '*'){
                    std::vector<std::string> ballCoords = Utils::splitstringbychar(extractSubstrings[i].substr(1), ",");
                    gameWindow.ballXPrev = gameWindow.ballX;
                    gameWindow.ballYPrev = gameWindow.ballY;
                    gameWindow.ballX = std::stof(ballCoords[0]);
                    gameWindow.ballY = std::stof(ballCoords[1]);
                    gameWindow.ballNeedsLerp = true;
                }
                if (extractSubstrings[i][0] == '$'){
                    std::vector<std::string> currentCoords = Utils::splitstringbychar(extractSubstrings[i].substr(1), ",");
                    gameWindow.newPlayerX = std::stof(currentCoords[0]);
                    gameWindow.newPlayerY = std::stof(currentCoords[1]);
                    gameWindow.otherPlayerNeedsLerp = true;
                }
                if (extractSubstrings[i][0] == '~'){
                    std::vector<std::string> currentCoords = Utils::splitstringbychar(extractSubstrings[i].substr(1), ",");
                    gameWindow.player1Score = std::stoi(currentCoords[0]);
                    std::cout<<gameWindow.player1Score<<"\n";
                    gameWindow.player2Score = std::stoi(currentCoords[1]);
                }
                if (extractSubstrings[i][0] == '?'){
                    std::vector<std::string> currentCoords = Utils::splitstringbychar(extractSubstrings[i].substr(1), ",");
                    if (!gameWindow.world.checkCollisionGeneral({std::stof(currentCoords[0]), gameWindow.y}, 50)){
                        gameWindow.x = std::stof(currentCoords[0]);
                    }
                    else {
                    }
                    if (!gameWindow.world.checkCollisionGeneral({gameWindow.x,std::stof(currentCoords[1])}, 50)){
                        gameWindow.y = std::stof(currentCoords[1]);
                    }
                    else {
                    }
                }
            }
            if (stringToSend.size() > 0){
                network.sendData(stringToSend);
            }
            data = network.receiveData();
        }
        if (foundConnection == 1){
            std::vector<std::string> data = Utils::splitstringbychar(connectionPage.name,"/");
            int connectionTry = 1;
            if (data.size() != 3){
                foundConnection = 0;
                continue;
            }
            try {
                connectionTry = network.init(data[0], std::stoi(data[1]));
            } catch (const std::exception& e) {
                foundConnection = 0;
                std::cerr << "Error during connection initialization: " << e.what() << std::endl;
            }
            if (connectionTry == 0){
                network.createSocket();
                gameWindow.setUsername(data[2]);
                //TODO: send server hello
                std::cout<<"socket created";
                foundConnection = 2;
            }
            else {
                foundConnection = 0;
            }
            //initialize connection to server
            //proceed to show game screen
        }

    }
    UnloadTexture(gameWindow.assets);
    RaylibCloseWindow();
    network.shutDown();
    return 0;
}