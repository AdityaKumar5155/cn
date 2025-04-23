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
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    FILE *file;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP File Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("File requested: %s\n", buffer);

        file = fopen(buffer, "rb");
        if (!file) {
            strcpy(buffer, "FILE_NOT_FOUND");
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
            continue;
        }

        // Send file in chunks
        while (!feof(file)) {
            int bytes = fread(buffer, 1, BUFFER_SIZE, file);
            sendto(sockfd, buffer, bytes, 0, (struct sockaddr *)&client_addr, addr_len);
        }

        // Send special EOF signal
        strcpy(buffer, "EOF");
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
        fclose(file);

        printf("File sent successfully.\n");
    }

    close(sockfd);
    return 0;
}
