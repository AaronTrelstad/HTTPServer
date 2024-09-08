#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8081
#define BUFFER_SIZE 1024

#define ERROR_404_PAGE "UI/404.html"
#define INDEX_PAGE "UI/index.html"
#define ME "UI/me.html"

void handle_client(int client_socket);
void send_response(int client_socket, const char* status, const char* content_type, const char *file_path);
char* read_file(const char* file_path);


#endif // SERVER_H
