#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void traverse(const char* path)
{
    DIR* directory;
    int status;
    struct dirent* entry;
    char* childPath;
    int fd;
    struct stat properties;

    if (path == NULL)
    {
        printf("path cannot be NULL\n");
        exit(EXIT_FAILURE);
    }

    directory = opendir(path);
    if (directory == NULL)
    {
        printf("Failed to open directory: %s\n", path);
        exit(EXIT_FAILURE);
    } 

    entry = readdir(directory);
    while(entry != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            childPath = (char*)malloc(strlen(path) + strlen(entry->d_name) + 2);
            if (childPath == NULL)
            {
                printf("Failed to allocate memory for child path.\n");
                exit(EXIT_FAILURE);
            }

            sprintf(childPath, "%s/%s", path, entry->d_name);
            printf("%s\n", childPath);

            status = stat(childPath, &properties);
            if (status == -1)
            {
                printf("Failed to read properties of %s\n", childPath);
                exit(EXIT_FAILURE);
            }

            if (S_ISDIR(properties.st_mode))
            {
                traverse(childPath);
            }

            free(childPath);
        }

        entry = readdir(directory);
    }

    status = closedir(directory);
    if (status == -1)
    {
        printf("Failed to close directory");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char* argv[])
{
    if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    traverse(argv[1]);

    exit(EXIT_SUCCESS);
}