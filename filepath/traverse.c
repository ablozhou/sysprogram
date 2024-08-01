#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void traverse(char *path)
{
    printf("entering: %s\n\n", path);
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        printf("dir %s doesn't exist", path);
    }
    struct dirent *entry = readdir(dir);

    while (entry != NULL)
    {
        printf("%s/%s\n", path, entry->d_name);
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, ".."))
        {
            char *childPath = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(childPath, "%s/%s", path, entry->d_name);
            traverse(childPath);
            free(childPath);
        }
        entry = readdir(dir);
    }
    closedir(dir);
}

void main()
{
    traverse(".");
}