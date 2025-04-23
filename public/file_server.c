#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];
    FILE *file;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    printf("Server listening on port %d...\n", PORT);
    client_fd = accept(server_fd, (struct sockaddr *)&address, &addr_len);
    printf("Client connected.\n");

    // Receive filename from client
    int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    buffer[len] = '\0';  // safely null-terminate
    printf("Client requested file: %s\n", buffer);

    file = fopen(buffer, "rb");
    if (file == NULL) {
        strcpy(buffer, "FILE_NOT_FOUND");
        send(client_fd, buffer, strlen(buffer), 0);
        printf("File not found.\n");
    } else {
        while (!feof(file)) {
            int n = fread(buffer, 1, BUFFER_SIZE, file);
            send(client_fd, buffer, n, 0);
        }
        printf("File sent successfully.\n");
        fclose(file);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
