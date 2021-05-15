#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*errors list:

implementation errors:
1)line 30 - implementing an int value to a pointer
2)line 31 - no memory for the "\0" character has been allocated
3)line 33 - for i=0 str[*x - i] is out of the string boundaries
4)
5)


proper programming errors:
1)line 24 - missing a NULL check for malloc
2)line 30 - can just write "else"
3)line 24 - even if sizeof(char) is equal 1 it's better to write it down
4)line 21 - need to implement a value to a pointer ASAP (use malloc the next line or at the same line)
5)line 23 - since we are using c99 "int i" can go inside the for loop
6)line 30 - need to check that the pointer x is not NULL


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
    if(x == NULL)
    {
        return NULL;
    }

    *x = strlen(str);

    char* str2 = malloc(sizeof(char) * (*(x) + 1));
    if(str2 == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < *x; i++)
    {
        str2[i] = str[*x - i - 1];
    }
    
    if (*x % 2 == 0) 
    {
        printf("%s", str);
    }
    else
    {
        printf("%s", str2);
    }
    return str2;
}