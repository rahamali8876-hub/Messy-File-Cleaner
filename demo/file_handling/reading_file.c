

#include <stdio.h>

int main(int argc, char const *argv[])
{
    /* code */
    // read data from file in c programming

    FILE *fptr;
    fptr = fopen("file2.txt", "r");
    if (fptr == NULL)
    {
        printf("Error: Could not open/create a file\n");
        return 1;
    }

    char ch;

    fclose(fptr);

    return 0;
}
