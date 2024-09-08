#include "server.h"

void send_response(int client_socket, const char* status, const char* content_type, const char *file_path) {
    printf("Sending response for file: %s\n", file_path);

    char *html_content = read_file(file_path);
    if (!html_content) {
        const char *error_message = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\n500 Internal Server Error";
        send(client_socket, error_message, strlen(error_message), 0);
        return;
    }

    size_t content_length = strlen(html_content);
    char response[2048];
    int response_len = snprintf(response, sizeof(response),
                                "HTTP/1.1 %s\r\n"
                                "Content-Type: %s\r\n"
                                "Content-Length: %zu\r\n"
                                "\r\n"
                                "%s",
                                status, content_type, content_length, html_content);

    if (response_len >= sizeof(response)) {
        printf("Response truncated.\n");
    }

    printf("Response sent:\n%s\n", response);

    ssize_t bytes_sent = send(client_socket, response, response_len, 0);
    if (bytes_sent < 0) {
        if (errno == EPIPE) {
            printf("Client closed the connection. Error: %s\n", strerror(errno));
        } else {
            perror("Send failed");
        }
    }

    free(html_content);
}

void handle_get_request(char *path, int client_socket) {
    printf("Handling GET request for path: %s\n", path);

    if (strcmp(path, "/") == 0) {
        send_response(client_socket, "200 OK", "text/html", INDEX_PAGE);
        return;
    }

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "UI%s", path);
    printf("Resolved file path: %s\n", file_path);

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("File not found, sending 404 response.\n");
        send_response(client_socket, "404 Not Found", "text/html", ERROR_404_PAGE);
    } else {
        printf("File found, sending content.\n");
        fclose(file);
        send_response(client_socket, "200 OK", "text/html", file_path);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("Read failed");
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';

    char method[16], path[256], version[16];
    int parsed = sscanf(buffer, "%15s %255s %15s", method, path, version);

    if (parsed < 3) {
        printf("Invalid request format\n");
        const char *bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 15\r\n\r\n400 Bad Request";
        send(client_socket, bad_request, strlen(bad_request), 0);
        close(client_socket);
        return;
    }

    if (strcmp(method, "GET") == 0) {
        handle_get_request(path, client_socket);
    } else {
        const char *method_not_allowed = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: 22\r\n\r\n405 Method Not Allowed";
        send(client_socket, method_not_allowed, strlen(method_not_allowed), 0);
    }

    close(client_socket);
}





