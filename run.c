#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define DOMAIN AF_INET
#define SERVER_IP "149.125.45.77"
#define PORT 8080

int main(int argc, char* argv[]) {
    WSADATA wsaData = {0};
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    printf("select socket type:\n0: server\n1+: client\n");
    int clientID;
    wscanf(L"%d", &clientID);
    if (clientID == 0) {
        printf("input number of clients: ");
        unsigned int numClients;
        scanf("%u", &numClients);

        SOCKET server = socket(DOMAIN, SOCK_STREAM, IPPROTO_TCP);
        if (server == INVALID_SOCKET) {
            wprintf(L"socket couldn't be opened (error: %d)\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        SOCKADDR_IN address;
        address.sin_family = DOMAIN;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        int addrlen;

        bind(server, (PSOCKADDR) &address, sizeof(address));
        listen(server, 5);
        SOCKET clients[numClients];
        for (int i = 0; i < numClients; i++) {
            clients[i] = accept(server, (PSOCKADDR) &address, &addrlen);
        }

        char buffer[1024] = {0};

        do {
            for (int i = 0; i < numClients; i++)
            {
                recv(clients[i], buffer, 1, 0);
                printf("%d", i);
            }
            if (strncmp(buffer, "", 1)) printf("%s\n", buffer);
        } while (strcmp(buffer, "q") != 0);
    return 0;
        
        closesocket(server);
        for (int i = 0; i < numClients; i++) {
            closesocket(clients[i]);
        }
    }
    else {
        SOCKET client = socket(DOMAIN, SOCK_STREAM, IPPROTO_TCP);
        if (client == INVALID_SOCKET) {
            wprintf(L"socket couldn't be opened (error: %d)\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        SOCKADDR_IN address = {.sin_family = DOMAIN, .sin_port = htons(PORT)};

        inet_pton(DOMAIN, SERVER_IP, &address.sin_addr);
        connect(client, (PSOCKADDR) &address, sizeof(address));

        char message[20] = {0};

        do {
            scanf("%s", message);
            send(client, message, sizeof(message), 0);
        } while (strcmp(message, "q") != 0);

        closesocket(client);
    }

    WSACleanup();
    return 0;
}