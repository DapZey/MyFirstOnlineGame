#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include "Utils.h"
#include "World.h"
struct Client {
    SOCKET socket;
    sockaddr_in address;
    int addressLength = sizeof(address);
    char buffer[40]; // Separate buffer for each client
    int x = 0;
    int y = 0;
    std::chrono::time_point<std::chrono::steady_clock> lastSendRecvTimeGeneral = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastSendRecvTimeRTT = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> RTTGap = std::chrono::steady_clock::now();
    int updateFreq = 0;
    int rttCount = 0;
    int currentRTT = 0;
    int averageRTT = 0;
    long long totalRTT = 0;
    bool centered = false;
    bool waiting = false;
    float oldBallX = 0;
    float oldBallY =0;
    int needToRespond = false;
};
std::vector<std::string> splitstringbychar(const std::string& input, const std::string& delimiters) {
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type pos;

    // Iterate until there are no more delimiters found
    while ((pos = input.find_first_of(delimiters, start)) != std::string::npos) {
        // Add the substring before the delimiter
        result.push_back(input.substr(start, pos - start));
        // Update starting position to skip the delimiter
        start = pos + 1;
    }

    // Add the last substring after the last delimiter (or the entire string if no delimiters)
    result.push_back(input.substr(start));

    return result;
}
void listenForStopCommand(std::atomic<bool>& running) {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "stop") {
            running = false;
        }
    }
}
bool containsChar(const std::string &str, char ch) {
    // Use the find method to check if the character is in the string
    return str.find(ch) != std::string::npos;
}
SOCKET createSocket(int port) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
        return INVALID_SOCKET;
    }
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in hint;
    hint.sin_addr.s_addr = INADDR_ANY;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);

    if (bind(sock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << "\n";
        closesocket(sock);
        return INVALID_SOCKET;
    }

    return sock;
}
int startWinsock(){
    WSADATA wsaData;
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }
    return 0;
}

int main() {
    startWinsock();
    // Create sockets for two clients
    SOCKET socket1 = createSocket(54000);
    SOCKET socket2 = createSocket(54001);
    if (socket1 == INVALID_SOCKET || socket2 == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    Client clients[2];
    clients[0].socket = socket1;
    clients[1].socket = socket2;

    std::atomic<bool> running(true);

    // Start the thread to listen for the stop command
    std::thread stopCommandThread(listenForStopCommand, std::ref(running));
    const std::string responseRTT = "%";
    const std::string checkRTT = "&";
    World world;
    while (running) {
        world.run();
        for (int i = 0; i < 2; ++i) {
            auto now = std::chrono::steady_clock::now();
            auto elapsedGeneral = std::chrono::duration_cast<std::chrono::milliseconds>(now - clients[i].lastSendRecvTimeGeneral);
            auto elapsedRTT = std::chrono::duration_cast<std::chrono::milliseconds>(now - clients[i].lastSendRecvTimeRTT);
            int otherClientIndex = (i == 0) ? 1 : 0;
            std::string stringToSendToCurrent;
            std::string stringToSendToOther;
            std::string data = clients[i].buffer;
            if (data.empty()){
//                std::cout<<"no data\n";
            }
            else {
                std::cout<< "received: "<<data<<"\n";
            }
            if (elapsedRTT.count() >= 1000 && clients[i].waiting == false && clients[i].centered == true) {
                clients[i].waiting = true;
                stringToSendToCurrent += checkRTT;
                clients[i].lastSendRecvTimeRTT = now;
                clients[i].RTTGap = now;
                std::string s ="?"+ std::to_string(clients[i].x)+","+ std::to_string(clients[i].y)+"?";
                stringToSendToCurrent +=s;
                std::cout<<"1 second passed, sending rtt check\n";
            }
            if (containsChar(data,'%')) {
                clients[i].needToRespond = true;
                std::cout<<"received rtt queue from player\n";
            }
            if (containsChar(data, '&')){
                clients[i].waiting = false;
                auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now - clients[i].RTTGap);
                clients[i].currentRTT = time.count();
                clients[i].rttCount++;
                clients[i].totalRTT += clients[i].currentRTT;
                clients[i].averageRTT = (int)(clients[i].totalRTT / clients[i].rttCount);
                clients[i].updateFreq = (int)(0.9 * clients[i].currentRTT + 0.1 * clients[i].averageRTT);
                std::cout<<"received rtt check response, frequency is"<<clients[i].updateFreq<<"\n";
            }
            if (containsChar(data,'*')){
                std::vector<std::string> coordinates = splitstringbychar(data.substr(data.find('*')+1,data.size()),",");
                Vector2 coords = {std::stof(coordinates[0]), std::stof(coordinates[1])};
                Vector2 currentClientCoords = {(float)clients[i].x, (float)clients[i].y};
                std::cout<<"received coordinates\n";
                if (Utils::Vector2Distance(coords, currentClientCoords) > 79 && clients[i].centered == true){
                    std::cout<<"coords invalid, rerouting player\n";
                }
                else {
                    clients[i].centered = true;
                    clients[i].x = std::stoi(coordinates[0]);
                    clients[i].y = std::stoi(coordinates[1]);
                    std::string s = "$"+std::to_string(clients[i].x)+","+ std::to_string(clients[i].y) +"$";
                    stringToSendToOther += s;
                    std::cout<<"applying and forwarding coords\n";
                }

            }
            if (clients[i].oldBallY != world.ball.y || clients[i].oldBallX != world.ball.x){
                clients[i].oldBallY = world.ball.y;
                clients[i].oldBallX = world.ball.x;
                std::string s = "*"+ std::to_string((int)clients[i].oldBallX)+","+std::to_string((int)clients[i].oldBallY)+"*";
                stringToSendToCurrent += s;
            }
            if (elapsedGeneral.count() >= clients[i].updateFreq) {
                if (clients[i].needToRespond){
                    stringToSendToCurrent += responseRTT;
                    clients[i].needToRespond = false;
                }
                if (!stringToSendToCurrent.empty()){
                    clients[i].lastSendRecvTimeGeneral = now;
                    int message = sendto(clients[i].socket, stringToSendToCurrent.c_str(), static_cast<int>(stringToSendToCurrent.size()) + 1, 0, (sockaddr*)&clients[i].address, clients[i].addressLength);
                    if (message == SOCKET_ERROR) {
                    }
                }
                if (!stringToSendToOther.empty()){
                    clients[i].lastSendRecvTimeGeneral = now;
                    int otherMessage = sendto(clients[otherClientIndex].socket, stringToSendToOther.c_str(), static_cast<int>(stringToSendToOther.size()) + 1, 0, (sockaddr*)&clients[otherClientIndex].address, clients[otherClientIndex].addressLength);
                    if (otherMessage == SOCKET_ERROR) {
                    }
                }
                if (!stringToSendToCurrent.empty() || !stringToSendToOther.empty()){
//                    std::cout<<"update freq valid, sending to current: "<<stringToSendToCurrent<<" and other "<<stringToSendToOther<<"\n";
                }
            }
            else {
//                std::cout<<"updateFrequency invalid, frequency is: "<<clients[i].updateFreq<<" but elapsed is: "<<elapsedGeneral.count()<<"\n";
            }
            std::memset(clients[i].buffer,0,sizeof clients[i].buffer);
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clients[i].address.sin_addr, clientIP, INET_ADDRSTRLEN);
            unsigned short clientPort = ntohs(clients[i].address.sin_port);
            int bytes = recvfrom(clients[i].socket, clients[i].buffer, sizeof(clients[i].buffer), 0, (sockaddr*)&clients[i].address, &clients[i].addressLength);
            if (bytes == SOCKET_ERROR) {
                int error = WSAGetLastError();
                if (error == WSAEWOULDBLOCK || error == WSAEMSGSIZE) {
                    continue;
                } else {
//                    std::cerr << "recvfrom failed with error: " << error << "\n";
                    continue;
                }
            }
        }
    }
    stopCommandThread.join();
    // Clean up
    closesocket(socket1);
    closesocket(socket2);
    WSACleanup();
    return 0;
}