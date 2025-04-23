#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];
    FILE *file;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Enter filename to request: ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\n")] = 0; // remove newline

    send(sock, filename, strlen(filename), 0);

    file = fopen("received_file", "wb");
    if (!file) {
        perror("File creation failed");
        return 1;
    }

    int bytes;
    while ((bytes = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strncmp(buffer, "FILE_NOT_FOUND", 14) == 0) {
            printf("Server says: FILE_NOT_FOUND\n");
            break;
        }
        fwrite(buffer, 1, bytes, file);
    }

    fclose(file);
    close(sock);
    printf("File received and saved as 'received_file'.\n");

    return 0;
}
