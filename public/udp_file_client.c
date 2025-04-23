#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    FILE *file;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    printf("Enter filename to request: ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\n")] = 0;

    sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr *)&server_addr, addr_len);

    file = fopen("received_file", "wb");
    if (!file) {
        perror("File creation failed");
        return 1;
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);

        if (strncmp(buffer, "FILE_NOT_FOUND", 14) == 0) {
            printf("Server: FILE_NOT_FOUND\n");
            remove("received_file");
            break;
        }

        if (strncmp(buffer, "EOF", 3) == 0) {
            printf("File received completely.\n");
            break;
        }

        fwrite(buffer, 1, bytes, file);
    }

    fclose(file);
    close(sockfd);
    return 0;
}
