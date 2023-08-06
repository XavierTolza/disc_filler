#ifndef FOLDER_COPY_H
#define FOLDER_COPY_H

// Function to create a directory recursively
int create_directory(const char *path);

// Function to copy a file from the source to destination
int copy_file(const char *src, const char *dst);

// Function to recursively copy a folder and its contents
void copy_folder(const char *src, const char *dst);

#endif /* FOLDER_COPY_H */
