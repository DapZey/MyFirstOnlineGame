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
#define MAX_INPUT_CHARS 28


int main(int argc, char* argv[]) {
    const int screenWidth = 1280;
    const int screenHeight = 960;

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
    while (!WindowShouldClose()) {
        if (foundConnection ==0){
            foundConnection = connectionPage.ShowPage();
        }
        else {
            gameWindow.runGame();
            auto now = std::chrono::steady_clock::now();
            auto elapsedGeneral = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendRecvTimeGeneral);
            auto elapsedRTT = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendRecvTimeRTT);
            if (elapsedGeneral.count() >= updateFreq + 1) {
                lastSendRecvTimeGeneral = now;
                network.sendCoordinates(gameWindow.x,gameWindow.y);
            }
            if (elapsedRTT.count() >= 10000) {
                lastSendRecvTimeRTT = now;
                RTTGap = now;
                network.sendRTT();
            }
            std::string coords = network.receiveData();
            std::vector<std::string> player2coords = Utils::splitstringbychar(coords, ",");
            if (coords[0] == '%'){
                auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now - RTTGap);
                currentRTT = time.count();
                rttCount++;
                totalRTT += currentRTT;
                averageRTT = (int)(totalRTT / rttCount);
                updateFreq = (int)(0.9 * currentRTT + 0.1 * averageRTT);
                std::cout << updateFreq << "\n";
            }
            else if(coords[0] == '&'){
                network.receiveRTT();
                if (player2coords.size() > 1) {
                    if (!gameWindow.world.checkCollisionGeneral({std::stof(Utils::splitstringbychar(player2coords[0], "&")[1]),gameWindow.y},50)){
                        gameWindow.x = std::stoi(Utils::splitstringbychar(player2coords[0], "&")[1]);
                        std::cout<<"compensated x"<<"\n";
                    }
                    else {
                        std::cout<<"failed compensation on x"<<"\n";
                    }
                    if (!gameWindow.world.checkCollisionGeneral({gameWindow.x,std::stof(player2coords[1])},50)){
                        gameWindow.y = std::stoi(player2coords[1]);
                        std::cout<<"compensated y"<<"\n";
                    }
                            else {
                        std::cout<<"failed compensation on y"<<"\n";
                    }
                }
            }
            else if (player2coords.size() > 1){
                gameWindow.newPlayerx = std::stoi(player2coords[0]);
                gameWindow.newPlayerY = std::stoi(player2coords[1]);
            }
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

    RaylibCloseWindow();
    network.shutDown();
    return 0;
}