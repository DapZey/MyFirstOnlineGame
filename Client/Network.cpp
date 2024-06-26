//
// Created by benra on 6/4/2024.
//

#include "Network.h"
#include <iostream>
#include <vector>
#include "cmath"
int Network::receiveRTT() {
    if (buffer[0] =='&') {
        int sendResult = sendto(output, "&", 2,0, (sockaddr*)&serverMessage, sizeof(serverMessage));
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "sendto failed with error: " << WSAGetLastError() << "\n";
        }

    }
    return 0;
}
int Network::receiveBall() {
    if (buffer[0] =='?') {
        int sendResult = sendto(output, "?", 2,0, (sockaddr*)&serverMessage, sizeof(serverMessage));
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "sendto failed with error: " << WSAGetLastError() << "\n";
        }
    }
    return 0;
}
std::string Network::receiveData() {
    int bytesReceived = recvfrom(output, buffer, sizeof(buffer), 0, (sockaddr*)&serverResponse, &serverResponseSize);
    if (bytesReceived == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
//            std::cerr << "recvfrom() failed: " << WSAGetLastError() << "\n";
            return "";
        }
    } else {
        return buffer;
    }
    return "";
}
void Network::sendData(std::string s) {
    int message = sendto(output, s.c_str(), static_cast<int>(s.size()) + 1, 0, (sockaddr*)&serverMessage, sizeof(serverMessage));
    if (message == SOCKET_ERROR) {
        std::cerr << "sendto() failed: " << WSAGetLastError() << "\n";
    }
}
void Network::sendCoordinates(float x, float y) {
    std::cout<<"sending coords\n";
    int xDir = std::round(x);
    int yDir = std::round(y);
    s = std::to_string(xDir) + "," + std::to_string(yDir);
    if (old != s) {
        old = s;
        int message = sendto(output, s.c_str(), static_cast<int>(s.size()) + 1, 0, (sockaddr*)&serverMessage, sizeof(serverMessage));
        if (message == SOCKET_ERROR) {
            std::cerr << "sendto() failed: " << WSAGetLastError() << "\n";
        }
    }
}
void Network::sendRTT() {
    int message = sendto(output, "%", 2, 0, (sockaddr*)&serverMessage, sizeof(serverMessage));
    if (message == SOCKET_ERROR) {
        std::cerr << "sendto() failed: " << WSAGetLastError() << "\n";
    }
}
int Network::startup() {
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }
    return 0;
}
int Network::createSocket() {
    output = socket(AF_INET, SOCK_DGRAM, 0);
    if (output == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << "\n";
        return 1;
    }
    u_long mode = 1;
    ioctlsocket(output, FIONBIO, &mode);
    serverResponseSize = sizeof(serverResponse);
    return 0;
}
void Network::shutDown() {
    closesocket(output);
    WSACleanup();
}
int Network::init(std::string ip, int port){
    serverMessage.sin_family = AF_INET;
    if (port == 1){
        serverMessage.sin_port = htons(54000);
    }
    else if (port == 2) {
        serverMessage.sin_port = htons(54001);
    }else {
        return 1;
    }

    if (inet_pton(AF_INET, ip.c_str(), &serverMessage.sin_addr) != 1) {
        std::cerr << "Invalid address. inet_pton failed\n";
        return 1;
    }
    return 0;
}