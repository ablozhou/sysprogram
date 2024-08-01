
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 4
void stringCopy(char *destination, char *source, unsigned int maxCharacters)
{
    if (destination == NULL || source == NULL)
    {
        perror("destination or source is NULL\n");
        exit(1);
    }
    for (int i = 0; i < maxCharacters; i++)
    {

        destination[i] = source[i];
    }
}
void swapCharacterArrays(char *x, char *y, unsigned int numCharacters)
{
    char t;
    for (int i = 0; i < numCharacters; i++)
    {
        t = x[i];
        x[i] = y[i];
        y[i] = t;
    }
}
void swapString(char *x, char *y)
{
    char t;
    if (strlen(x) != strlen(y))
    {
        printf("the length of two string is not equal, swap failed.\n");
        return;
    }
    while (*x != '\0')
    {
        t = *x;
        *x = *y;
        *y = *x;
    }
}
void printArray(int *x, unsigned int numIntegers)
{
    for (int i = 0; i < numIntegers; i++)
    {
        printf("%d ", *x);
        x++;
    }
    printf("\n");
}

void modifyStrings(char *items[], int (*visitor)(int))
{

    // for (int i = 0; i < size; i++)
    //  {
    //      for (int j = 0; j < strlen(items[i]); j++)
    //      {
    //          items[i][j] = visitor(items[i][j]);
    //      }
    //  }
    // int i = 0;
    // while (items[i] != NULL)
    // {
    //     printf("i %s\n", items[i]);
    //     for (int j = 0; j < strlen(*items); j++)
    //     {
    //         items[i][j] = visitor(items[i][j]);
    //     }
    //     printf("e %s\n", items[i]);
    //     i++;
    // }

    while (*items != NULL)
    {
        printf("i %s\n", *items);
        for (int j = 0; j < strlen(*items); j++)
        {
            *items[j] = visitor(*items[j]);
        }
        printf("e %s\n", *items);
        items++;
    }
}
void printArray1(char **x, int size)
{
    // for (int i = 0; i < size; i++)
    // {
    //     printf("%s\n", x[i]);
    // }
    while (*x != NULL)
    {
        printf("%s\n", *x);
        x++;
    }
}
void main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s string1 ...\n", argv[0]);
        exit(1);
    }
    char a[SIZE] = "abcd"; //{1, 2, 3, };
    char b[SIZE] = "efgh"; //{5, 6, 7, 8};
    printf("before a: %s\n", a);
    printf("before b: %s\n", b);
    swapString((char *)a, (char *)b);
    printf("a: %s\n", a);
    printf("b: %s\n", b);
    // int c[SIZE] = {1234, 2, 3, 9};
    // int d[SIZE] = {5, 6, 7, 8};
    //  printArray((char *)a, sizeof(int) * SIZE);
    //  printArray((char *)b, sizeof(int) * SIZE);

    // 4.
    //  printArray(a, SIZE);
    //  printArray(b, SIZE);

    // swapCharacterArrays((char *)a, (char *)b, sizeof(int) * SIZE);

    // printArray(a, SIZE);
    // printArray(b, SIZE);

    // printArray((char *)a, sizeof(int) * SIZE);
    // printArray((char *)b, sizeof(int) * SIZE);
    //--
    // Segmentation fault (core dumped)
    // char *str1[2] = {"Abc d", "Hello World"};
    int size = argc;
    char *strs[size];
    for (int i = 0; i < size - 1; i++)
    {
        strs[i] = malloc(strlen(argv[i + 1]) + 1);
        strcpy(strs[i], argv[i + 1]);
    }
    strs[size - 1] = NULL;

    // printArray1(argv, argc);
    //  modifyStrings(strs, size, toupper);
    modifyStrings(strs, toupper);
    printf("bf %s\n", strs[0]);
    printArray1(strs, size - 1);
    printf("af %s\n", strs[0]);
    //  modifyStrings(strs, size, tolower);
    modifyStrings(strs, tolower);
    printf("b1 %s\n", strs[0]);
    printArray1(strs, size - 1);
    printf("af1 %s\n", strs[0]);
    exit(0);
}