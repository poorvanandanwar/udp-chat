#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

SOCKET sockfd;
struct sockaddr_in serverAddr;
int addrLen = sizeof(serverAddr);

void receiveMessages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        printf("\n%s\n", buffer);
        printf("You: ");
        fflush(stdout);
    }
}

int main() {
    WSADATA wsa;
    char buffer[BUFFER_SIZE], name[50];

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline

    // Send registration
    char regMsg[BUFFER_SIZE];
    snprintf(regMsg, BUFFER_SIZE, "REGISTER:%s", name);
    sendto(sockfd, regMsg, strlen(regMsg), 0, (struct sockaddr*)&serverAddr, addrLen);

    // Start receiver thread
    _beginthread(receiveMessages, 0, NULL);

    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // remove newline
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, addrLen);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}

