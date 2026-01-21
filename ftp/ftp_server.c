/* ===== Required header files ===== */
#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit(), EXIT_FAILURE
#include <string.h>     // strlen(), memset()
#include <unistd.h>     // close()
#include <arpa/inet.h>  // socket functions and structures

/* ===== Constant definitions ===== */
#define PORT 8080              // Port number for server
#define BUFFER_SIZE 1024       // Size of data buffer

/* ===== Function to handle one client ===== */
void handle_client(int client_socket) {

    /* Buffer to store file data */
    char buffer[BUFFER_SIZE] = {0};

    /* Buffer to store requested filename */
    char filename[BUFFER_SIZE] = {0};

    /* Receive filename from client */
    int bytes_received = recv(client_socket, filename, BUFFER_SIZE, 0);

    /* Check if receiving failed */
    if (bytes_received < 0) {
        perror("Error receiving filename");
        close(client_socket);   // Close client socket
        return;
    }

    /* Open the requested file in read mode */
    FILE *file = fopen(filename, "r");

    /* If file does not exist */
    if (file == NULL) {
        char *error_message = "File not found";
        send(client_socket, error_message, strlen(error_message), 0);
        perror("File not found");
    } 
    else {
        /* Read file line by line and send to client */
        while (fgets(buffer, BUFFER_SIZE, file) != NULL) {

            /* Send file data to client */
            send(client_socket, buffer, strlen(buffer), 0);

            /* Clear buffer for next read */
            memset(buffer, 0, BUFFER_SIZE);
        }

        printf("File sent to client\n");
        fclose(file);   // Close file
    }

    /* Close connection with client */
    close(client_socket);
}

/* ===== Main function ===== */
int main() {

    int server_socket, client_socket;

    /* Structures to store server and client address info */
    struct sockaddr_in server_addr, client_addr;

    socklen_t addr_len = sizeof(client_addr);

    /* Create TCP socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Check if socket creation failed */
    if (server_socket == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    /* Configure server address */
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept any IP
    server_addr.sin_port = htons(PORT);       // Convert port to network byte order

    /* Bind socket to IP and port */
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    /* Start listening for client connections */
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    /* Server runs forever */
    while (1) {

        /* Accept a client connection */
        client_socket = accept(server_socket,
                               (struct sockaddr *)&client_addr,
                               &addr_len);

        /* Check if accept failed */
        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        /* Handle client request */
        handle_client(client_socket);
    }

    /* Close server socket */
    close(server_socket);
    return 0;
}
