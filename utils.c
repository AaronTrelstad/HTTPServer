#include "server.h"

char* read_file(const char* filepath) {
    printf("Attempting to open file: %s\n", filepath);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0) {
        printf("File size is zero or negative.\n");
        fclose(file);
        return NULL;
    }

    char *file_content = malloc(file_size + 1);
    if (!file_content) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    fclose(file);
    printf("Read file successfully: %s\n", filepath);
    return file_content;
}

