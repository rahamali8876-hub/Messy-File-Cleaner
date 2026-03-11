#include <stdio.h>
// creteing data into file

int main()
{
    /* code */
    FILE *fptr;

    fptr = fopen("file2.txt", "w");
    if (fptr == NULL)
    {
        printf("Error: Could not open file\n");
        return 1;
    }
    fprintf(fptr, "hello world");

    fclose(fptr);

    return 0;
}
