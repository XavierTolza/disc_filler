#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "directory.h"

// Function to create a directory recursively
int create_directory(const char *path) {
    char buffer[256];
    char *p = NULL;
    size_t len;

    snprintf(buffer, sizeof(buffer), "%s", path);
    len = strlen(buffer);
    if (buffer[len - 1] == '/')
        buffer[len - 1] = 0;
    for (p = buffer + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(buffer, S_IRWXU);
            *p = '/';
        }
    return mkdir(buffer, S_IRWXU);
}

// Function to copy a file from the source to destination
int copy_file(const char *src, const char *dst) {
    FILE *source, *destination;
    int ch;

    source = fopen(src, "rb");
    if (source == NULL) {
        perror("Error opening source file");
        return 0;
    }

    destination = fopen(dst, "wb");
    if (destination == NULL) {
        fclose(source);
        perror("Error creating destination file");
        return 0;
    }

    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);
    return 1;
}

// Function to recursively copy a folder and its contents
void copy_folder(const char *src, const char *dst) {
    DIR *dir;
    struct dirent *entry;
    char src_path[512], dst_path[512];

    dir = opendir(src);
    if (dir == NULL) {
        perror("Error opening source directory");
        return;
    }

    create_directory(dst);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
            snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);

            if (entry->d_type == DT_DIR) {
                copy_folder(src_path, dst_path);
            } else if (entry->d_type == DT_REG) {
                copy_file(src_path, dst_path);
            }
        }
    }

    closedir(dir);
}

