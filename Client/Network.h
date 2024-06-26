//
// Created by benra on 6/4/2024.
//

#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <chrono>
class Network {
    WSADATA wsaData;
    int iResult;
    sockaddr_in serverMessage;
    SOCKET output;
    char buffer[40];
    sockaddr_in serverResponse;

    int serverResponseSize;
    std::string s;
    std::string old = "";
public:
    void shutDown();
    int init(std::string ip, int port);
    int startup();
    int createSocket();
    void sendCoordinates(float x, float y);
    std::string receiveData();
    void sendRTT();
    int receiveRTT();
    int receiveBall();
    void sendData(std::string s);
};


#endif //CLIENT_NETWORK_H