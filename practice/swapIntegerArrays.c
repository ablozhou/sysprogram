#include <stdio.h>
#include <stdlib.h>

void swapIntegerArrays(int *x, int *y, unsigned int numIntegers)
{
    int t;
    if (x == NULL || y == NULL)
    {
        perror("x or y is NULL\n");
        exit(1);
    }
    for (int i = 0; i < numIntegers; i++)
    {
        // t = *x;
        // *x = *y;
        // *y = t;
        // x++;
        // y++;
        t = x[i];
        x[i] = y[i];
        y[i] = t;
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
void main(int argc, char *argv[])
{
    int a[4] = {1, 2, 3, 4};
    int b[4] = {5, 6, 7, 8};
    printArray(a, 4);
    printArray(b, 4);

    swapIntegerArrays(a, b, 4);

    printArray(a, 4);
    printArray(b, 4);
    exit(0);
}