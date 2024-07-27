#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, BUFFER_SIZE);

    char method[16], path[256], version[16];
    sscanf(buffer, "%s %s %s", method, path, version);

    if (strcmp(method, "GET") == 0) {
        char *file_path = path + 1;
        FILE *file = fopen(file_path, "r");

        if (file == NULL) {
            char *not_found_response = "HTTP/1.1 404 Not Found\r\n"
                                       "Content-Type: text/html\r\n\r\n"
                                       "<html><body><h1>404 Not Found</h1></body></html>";
            write(client_socket, not_found_response, strlen(not_found_response));
        } else {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *file_content = malloc(file_size);
            fread(file_content, 1, file_size, file);

            char header[256];
            sprintf(header, "HTTP/1.1 200 OK\r\n"
                            "Content-Length: %ld\r\n"
                            "Content-Type: text/html\r\n\r\n", file_size);

            write(client_socket, header, strlen(header));
            write(client_socket, file_content, file_size);

            fclose(file);
            free(file_content);
        }
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind Failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen Failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept Failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        handle_client(client_socket);
    }
}
