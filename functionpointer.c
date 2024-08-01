#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ctype.h> //islower, toupper
//void modifyStrings(char* items[], int (*visitor)(int))
// must mofify to below, because in function parameter, we can't check the length of the array in c.
void modifyStrings(char *items[], int len, int (*visitor)(int))
{
    // int len = sizeof(items)/sizeof(*items);

    for (int i = 0; i < len; i++)
    {
        char *s = items[i];
        // printf("%s\n",items[i]);
        while (*s != '\0')
        {
            *s = visitor(*s);

            s++;
        }
        // printf("%s\n",items[i]);
    }
}
void printStringArray(char *items[], int len)
{

    for (int i = 0; i < len; i++)
    {

        printf("%s ", items[i]);
    }
    printf("\n");
}

void printUsage(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s string1 string2 ...\n", argv[0]);
        exit(1);
    }
}

void main(int argc, char *argv[])
{
    // warning we cannot modify the const string array directory, or else it would cause core dump
    // const char* items[] = {"Hello", "World!"};
    printUsage(argc, argv);
    // int len = sizeof(items)/sizeof(*items);
    int len = argc - 1;
    char **items = &argv[1];
    printf("before:");
    printStringArray(items, len);
    modifyStrings(items, len, toupper);
    printf("after:");
    printStringArray(items, len);
}