/* ===== Required header files ===== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* ===== Constant definitions ===== */
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {

    int client_socket;

    /* Structure to store server address */
    struct sockaddr_in server_addr;

    /* Buffer to receive file data */
    char buffer[BUFFER_SIZE] = {0};

    /* Buffer to store filename */
    char filename[BUFFER_SIZE] = {0};

    /* Create TCP socket */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Check if socket creation failed */
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* Configure server address */
    server_addr.sin_family = AF_INET;               // IPv4
    server_addr.sin_port = htons(PORT);             // Server port
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    /* Connect to server */
    if (connect(client_socket,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    /* Ask user for filename */
    printf("Enter the filename to request: ");
    scanf("%s", filename);

    /* Send filename to server */
    send(client_socket, filename, strlen(filename), 0);

    /* Receive and print file content */
    printf("File content received:\n");

    while (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {

        /* Print received data */
        printf("%s", buffer);

        /* Clear buffer */
        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("\n");

    /* Close socket */
    close(client_socket);
    return 0;
}
