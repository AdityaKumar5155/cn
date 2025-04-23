#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE], message[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("UDP server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        printf("Client: %s\n", buffer);

        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);
        sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&client_addr, addr_len);

        if (strncmp(message, "exit", 4) == 0) break;
    }

    close(sockfd);
    return 0;
}
