#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>
#include <vector>

struct Client {
    SOCKET socket;
    sockaddr_in address;
    int addressLength = sizeof(address);
    char buffer[20]; // Separate buffer for each client
    int x;
    int rttFromServer;
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

    while (running) {
        for (int i = 0; i < 2; ++i) {
            int bytes = recvfrom(clients[i].socket, clients[i].buffer, sizeof(clients[i].buffer), 0, (sockaddr*)&clients[i].address, &clients[i].addressLength);
            if (bytes == SOCKET_ERROR) {
                int error = WSAGetLastError();
                if (error == WSAEWOULDBLOCK || error == WSAEMSGSIZE) {
                    // Ignore non-fatal errors
                    continue;
                } else {
                    std::cerr << "recvfrom failed with error: " << error << "\n";
                    continue;
                }
            }

            // Send the received message to the other client
            int otherClientIndex = (i == 0) ? 1 : 0;
            if (clients[i].buffer[0] =='%') {
                int sendResult = sendto(clients[i].socket, clients[i].buffer, bytes, 0, (sockaddr*)&clients[i].address, clients[i].addressLength);
                if (sendResult == SOCKET_ERROR) {
                    std::cerr << "sendto failed with error: " << WSAGetLastError() << "\n";
                }
            }
            else {
                int sendResult = sendto(clients[otherClientIndex].socket, clients[i].buffer, bytes, 0, (sockaddr*)&clients[otherClientIndex].address, clients[otherClientIndex].addressLength);
                if (sendResult == SOCKET_ERROR) {
                    std::cerr << "sendto failed with error: " << WSAGetLastError() << "\n";
                }
            }

            // Log the received message
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clients[i].address.sin_addr, clientIP, INET_ADDRSTRLEN);
            unsigned short clientPort = ntohs(clients[i].address.sin_port);
            std::cout << "Received data from " << clientIP << ":" << clientPort << ": " << clients[i].buffer << "\n";
        }
    }
    stopCommandThread.join();
    // Clean up
    closesocket(socket1);
    closesocket(socket2);
    WSACleanup();
    return 0;
}
