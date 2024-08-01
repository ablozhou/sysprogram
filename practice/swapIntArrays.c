#include <stdio.h>

void swapIntegerArrays(int arr1[], int arr2[], int len)
{

    int tmp = 0;
    for (int i = 0; i < len; i++)
    {
        tmp = arr1[i];
        arr1[i] = arr2[i];
        arr2[i] = tmp;
    }
}
void printIntArray(int arr[], int len)
{
    // printf("sizeof(arr):%lu", sizeof(arr));
    for (int i = 0; i < len; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
int main()
{
    int arr1[4] = {1, 2, 3, 4};
    int arr2[4] = {5, 6, 7, 8};
    int len = sizeof(arr1) / sizeof(arr1[0]); // total size of bytes / unite size of bytes
    printf(" sizeof(arr1) %lu, sizeof(arr1[0]) %lu, len:%d", sizeof(arr1), sizeof(arr1[0]), len);
    printf("before:\n");
    printIntArray(arr1, len);
    printIntArray(arr2, len);

    swapIntegerArrays(arr1, arr2, len);
    printf("after:\n");
    printIntArray(arr1, len);
    printIntArray(arr2, len);
    return 0;
}