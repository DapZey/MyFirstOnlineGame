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
    const int screenWidth = 800;
    const int screenHeight = 600;

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
             if (coords[0] == '%'){
                 auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now - RTTGap);
                 updateFreq = time.count();
                 std::cout<< time.count()<<"/n";
             }
             else if(coords[0] == '&'){
                 network.receiveRTT();
             }
             std::vector<std::string> player2coords = Utils::splitstringbychar(coords, ",");
             if (player2coords.size() > 1){
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
