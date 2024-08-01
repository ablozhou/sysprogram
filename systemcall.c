// strace a.out
#include <stdio.h>
int main()
{
    FILE *fp;
    fp = fopen("/tmp/syscall.txt", "w+");
    if (fp == NULL)
    {
        perror("\t|Error opening file!\n");
        perror("\t|Terminating program!\n");
        return 1;
    }

    fprintf(fp, "\t|Using fprint to write to a file!\n");

    fputs("\t|Using fputs to write to a file!!\n", fp);

    fclose(fp);

    return 0;
}