/*
Create a program showallcontents.c that accepts a path to a directory as command
line argument and prints out the names of all files and directories contained in it.
Suppose the folder /home/a/csci7645 contains folders chapter1 and chapter2.
Suppose chapter1 contains files program1.c and program2.c and chapter2 contains files
program3.c and program4.c. Then, running the following:
      ./showAllContents /home/a/csci7645
should produce the following output:
      chapter1
          program1.c
          program2.c
      chapter2
          program3.c
          program4.c
where the tab character is used for indentation.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
int isLocalOrParentDir(char *path)
{
    if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
    {
        return 1;
    }
    return 0;
}
void showAllContents(char *path, int indent)
{
    if (path == NULL)
    {
        printf("error path\n");
        exit(1);
    }

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        printf("opendir error: path %s\n", path);
        exit(1);
    }
    struct dirent *entry =
        readdir(dir);
    while (entry != NULL)
    {
        if (isLocalOrParentDir(entry->d_name))
        {
            // printf("a %s\n", entry->d_name);
            entry = readdir(dir);
            continue;
        }

        for (int i = 0; i < indent; i++)
            printf("\t");

        printf("%s\n", entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            // if (isLocalOrParentDir(entry->d_name))
            // {
            //     entry = readdir(dir);
            //     continue;
            // }
            char *newPath = malloc(strlen(entry->d_name) + strlen(path) + 2);
            sprintf(newPath, "%s/%s", path, entry->d_name);
            showAllContents(newPath, indent + 1);
            free(newPath);
        }

        entry = readdir(dir);
    }
    closedir(dir);
}
void printUsage(int argc, char *argv[])
{
    if (argc < 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s path\n", argv[0]);
        exit(1);
    }
}

void main(int argc, char *argv[])
{
    printUsage(argc, argv);

    showAllContents(argv[1], 1);
}