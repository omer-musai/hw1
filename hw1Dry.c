#include <stdbool.h>
#include <stdio.h>

typedef struct node_t {
 int x;
 struct node_t *next;
} *Node;

typedef enum {
 SUCCESS=0,
 MEMORY_ERROR,
 EMPTY_LIST,
 UNSORTED_LIST,
 NULL_ARGUMENT,
} ErrorCode;

//functions declaration
bool isListSorted(Node list);
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code);
ErrorCode mergeSortedLists_aux(Node smallList, Node bigList);


Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code)
{
    *error_code = NULL_ARGUMENT;

    if(list1->x > list2->x)
    {
        *error_code = mergeSortedLists_aux(list2, list1);
        return list2;
    }
    else
    {
        *error_code = mergeSortedLists_aux(list1, list2);
        return list1;
    }
}

ErrorCode mergeSortedLists_aux(Node smallList, Node bigList)
{
    //base case
    if(!smallList->next)
    {
        smallList->next = bigList;
        return SUCCESS;
    }

    //check if the lists are sorted and not empty
    if(!smallList || !bigList)
    {
        return EMPTY_LIST;
    }

    if(!isListSorted(smallList) || !isListSorted(bigList))
    {
        return UNSORTED_LIST;
    }

    Node current = malloc(sizeof(*current));
    if(!current)
    {
        return MEMORY_ERROR;
    }

    current = smallList;

     while(current->x <= bigList->x && current->next->x <= bigList->x)
    {
        current = current->next;
    }

    
    Node tmp = malloc(sizeof(*tmp));
    if(!tmp)
    {
        return MEMORY_ERROR;
    }

    //placeholder in order to not lose the list
    tmp = current->next;

    //link to the other list
    current->next = bigList;

    free(current);

    //solve the same problem but smaller :)
    ErrorCode condition = mergeSortedLists_aux(bigList, tmp);
    
    free(tmp);

    return condition;
}