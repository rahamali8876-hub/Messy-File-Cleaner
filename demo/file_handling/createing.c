#include <stdio.h>

int main()
{
    // creating files using c programming
    FILE *fptr;
    fptr = fopen("file.txt", "w");
    fclose(fptr);

    return 0;
}
