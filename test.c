// Test program for mypreprocessor
#include <stdio.h>
#include "myheader.h"

#define PI 3.14
#define SQUARE(x) ((x)*(x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define AREA(r) (PI * SQUARE(r))

/* multi line
   comment here */
int main()
{
    int arr[MAX_SIZE];            // array declaration
    float a = AREA(5);            /* macro chained expansion */
    int m = MAX(SQUARE(2), 7);
    printf("PI is // not a comment, PI\n");
    return 0;
}