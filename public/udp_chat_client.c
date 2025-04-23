#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE], message[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    while (1) {
        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);
        sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&server_addr, addr_len);

        if (strncmp(message, "exit", 4) == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len);
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
