#include <iostream>
#include <string>
#include <thread>

#include <cstring>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DOMAIN AF_INET
// #define SERVER_IP "149.125.167.136"
// #define SERVER_IP "149.125.45.77"
#define SERVER_IP "127.0.0.1"
#define PORT 8081

using std::cout, std::cerr, std::cin, std::endl;

void MultithreadedServer();
void Client();
int setup_server(sockaddr_in*);
void get_data(int client);

int main(int argc, char* argv[]) {
    cout << "select socket type:\n\t0: server\n\t1+: client" << endl;
    std::size_t socketType;
    cin >> socketType;
    if (socketType == 0) {
        // PollServer();
        MultithreadedServer();
    }
    else {
        Client();
    }
    return 0;
}

void MultithreadedServer() {
    sockaddr_in address;
    socklen_t addrlen;
    int server = setup_server(&address), numClients;

    cout << "number of clients: ";
    cin >> numClients;

    int clients[numClients];
    std::thread threads[numClients];

    for (int i = 0; i < numClients; i++) {
        clients[i] = accept(server, (sockaddr*) &address, &addrlen);
        threads[i] = std::thread(get_data, clients[i]);
    }

    for (int i = 0; i < numClients; i++) {
        threads[i].join();
    }

    close(server);

    for (int i = 0; i < numClients; i++) {
        close(clients[i]);
    }
}

void Client() {
    int client = socket(DOMAIN, SOCK_STREAM, IPPROTO_TCP);
    if (client < 0) {
        perror("socket couldn't be opened");
        exit(1);
    }

    sockaddr_in address = {.sin_family = DOMAIN, .sin_port = htons(PORT)};

    inet_pton(DOMAIN, SERVER_IP, &address.sin_addr);
    connect(client, (sockaddr*) &address, sizeof(address));

    char message[32] = {0};

    do {
        cin >> message;
        send(client, &message, sizeof(message), 0);
    } while (message[0] != 'q');

    close(client);
}

int setup_server(sockaddr_in* address) {
    int server = -1;

    /* creates socket for server */
    if ((server = socket(DOMAIN, SOCK_STREAM, 0)) < 0) {
        perror("call to socket() failed");
        exit(EXIT_FAILURE);
    }

    address->sin_family = DOMAIN;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(server, (sockaddr*) address, sizeof(*address)) < 0) {
        perror("bind() failed");
        close(server);
        exit(EXIT_FAILURE);
    }

    if (listen(server, 32) < 0) {
        perror("listen() failed");
        close(server);
        exit(EXIT_FAILURE);
    }
    
    return server;
}

void get_data(int client) {
    char buffer[32] = {0};
    // char buffer;

    do {
        int valread = recv(client, &buffer, 31, 0);
        cout << std::string(buffer).substr(0, valread) << endl;
    } while (buffer[0] != 'q');
}

