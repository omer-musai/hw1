#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
errors list:

Implementation errors:

1) x has to be dereferenced to have a value placed in it. ("x = strlen(...")
2) x % 2 == 0 needs to be x % 2 != 0.
3) malloc size should be + 1. (This is to account for the null terminator.)
4) Should be [*x - 1 - i]. (Both to capture the first character, and to avoid read violation.)
5) Check if x is NULL. (Instructions imply that it could be.)


Proper programming errors:

1) Need to check malloc's return value.
2) Need to add a null terminator. 
3) Make the second "if" a simple "else".
4) Need to implement a value to a pointer ASAP (use malloc the next line or at the same line).
5) Since we are using c99 "int i" can go inside the for loop.
6) Curly brackets around loop & condition blocks. (Convention.)
7) malloc(*x) -> malloc(sizeof(*str) * (*x)) though it should work either way.

*/

//THE CODE BEFORE CHANGES
char* foo(char* str, int* x) {
    char* str2;
    int i;
    x = strlen(str);
    str2 = malloc(*x);
    for (i = 0; i < *x; i++)
         str2[i] = str[*x - i];
    if (*x % 2 == 0) {
        printf("%s", str);
    }
    if (*x % 2 != 0)
    {
        printf("%s", str2);
    }
    return str2;
 }

//THE FIXED CODE
char* foo(char* str, int* x)
{
    if (str == NULL)
    {
        return NULL;
    }

    int length = strlen(str);
    if (x != NULL)
    {
        *x = length;
    }

    char* str2 = malloc(sizeof(char) * (*(x) + 1));
    if(str2 == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        str2[i] = str[length - i - 1];
    }
    str2[length] = '\0';

    if (length % 2 != 0)
    {
        printf("%s", str);
    }
    else
    {
        printf("%s", str2);
    }
    return str2;
}