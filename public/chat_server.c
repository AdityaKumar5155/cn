#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server is listening on port %d...\n", PORT);
    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Client: %s\n", buffer);

        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(new_socket, message, strlen(message), 0);

        if (strncmp(message, "exit", 4) == 0) break;
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
