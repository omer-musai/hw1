#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_t {
    int x;
    struct node_t *next;

} *Node;

typedef enum {
    SUCCESS=0,
    MEMORY_ERROR,
    UNSORTED_LIST,
    NULL_ARGUMENT,

} ErrorCode;

//Function declarations:

/* Receives sorted linked lists; returns a merged list and stores error code in error_code. */
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code);

/* Performs the actual merging. */
ErrorCode mergeSortedLists_aux(Node list1, Node list2, Node mergedList);

/* Frees a null-terminated linked list. */
void freeList(Node list);

/* Creates a node and returns an error code (or SUCCESS if no error had occurred.) */
ErrorCode createNode(Node mergedList);

Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code)
{
    assert(error_code != NULL);

    *error_code = SUCCESS;


    if (list1 == NULL || list2 == NULL)
    {
        *error_code = NULL_ARGUMENT;
    }
    else if (!isListSorted(list1) || !isListSorted(list2))
    {
        *error_code = UNSORTED_LIST;
    }

    if (*error_code != SUCCESS)
    {
        return NULL;
    }

    Node mergedList = malloc(sizeof(*mergedList));

    *error_code = mergeSortedLists_aux(list1, list2, mergedList);
    if (*error_code != SUCCESS)
    {
        freeList(mergedList);
        return NULL;
    }

    return mergedList;
}


ErrorCode mergeSortedLists_aux(Node list1, Node list2, Node mergedList)
{
    ErrorCode memory_error = SUCCESS;
    Node prev = mergedList;
    while (list1 != NULL && list2 != NULL && memory_error == SUCCESS)
    {
        if(list1->x > list2->x)
        {
            mergedList->x = list2->x;
            list2 = list2->next;
        }
        else
        {
            mergedList->x = list1->x;
            list1 = list1->next;
        }

        memory_error = createNode(mergedList);
        prev = mergedList;
        mergedList = mergedList->next;
    }

    while (list1 != NULL && memory_error == SUCCESS)
    {
        mergedList->x = list1->x;
        list1 = list1->next;

        memory_error = createNode(mergedList);
        prev = mergedList;
        mergedList = mergedList->next;
    }

    while (list2 != NULL && memory_error == SUCCESS)
    {
        mergedList->x = list2->x;
        list2 = list2->next;

        memory_error = createNode(mergedList);
        prev = mergedList;
        mergedList = mergedList->next;
    }

    if (memory_error == SUCCESS)
    {
        prev->next = NULL;
        free(mergedList); //Freeing the excess element.
    }

    return memory_error;
}

void freeList(Node list)
{
    if (list == NULL)
    {
        return;
    }

    Node temp;
    while (list != NULL)
    {
        temp = list->next;
        free(list);
        list = temp;
    }
}

ErrorCode createNode(Node mergedList)
{
    if(mergedList == NULL)
    {
        return NULL_ARGUMENT;
    }
    mergedList->next = malloc(sizeof(*mergedList));
    if (mergedList->next == NULL)
    {
        return MEMORY_ERROR;
    }
    return SUCCESS;
}


Node createList(int arr[], int len)
{
    Node list = malloc(sizeof(*list)), next = list, prev = list;
    for (int cnt = 0; cnt < len; ++cnt)
    {
        next->x = arr[cnt];
        createNode(next);
        prev = next;
        next = next->next;
    }
    free(next);
    prev->next = NULL;

    return list;
}

void printList(Node list)
{
    while (list != NULL)
    {
        printf("%d ", list->x);
        list = list->next;
    }
    printf("\n");
}


int main() {
    int arr[] = {1,2,3,4,5}, arr2[] = {1,2,7,8}, len1 = 5, len2 = 4;
    Node list = createList(arr, len1), list2 = createList(arr2, len2);
    printList(list);
    printList(list2);

    ErrorCode error;
    Node mergedList = mergeSortedLists(list, list2, &error);
    printf("%d\n", error);
    printList(mergedList);

    freeList(list);
    freeList(list2);
    freeList(mergedList);

    return 0;
}
