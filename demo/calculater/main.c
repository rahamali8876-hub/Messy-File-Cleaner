#include <stdio.h>
// #include "cal.c"

// int main()
// {
//     /* code */
//     int a = 10;
//     int b = 20;
//     int c = add(a, b);
//     printf("c = %d\n", c);
//     int dd = sub(a, b);
//     printf("dd = %d\n", dd);
//     int e = mul(a, b);
//     printf("e = %d\n", e);
//     float ff = div(a, b);
//     printf("ff = %f\n", ff);
//     return 0;
// }

void count()
{
    int myNum = 0; // Keeps its value between calls
    myNum++;
    printf("num = %d\n", myNum);
}

int main()
{
    count();
    count();
    count();
    return 0;
}