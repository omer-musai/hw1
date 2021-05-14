
#include "map.h"
#include <stdio.h>
#include <stdlib.h>


typedef enum SearchResults {
    FOUND,
    NOT_FOUND
} SearchResults;

typedef struct LinkedListNode
{
    MapDataElement value;
    MapKeyElement key;
    struct LinkedListNode *next;
} MapNode;

//functions declaration
MapNode* findPreviousElementPosition(Map map, MapKeyElement key, SearchResults* results);

struct Map_t
{
    MapNode *iterator;
    MapNode *elements;
    
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    compareMapKeyElements compareElements;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
};

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
                Map map = malloc(sizeof(*map));
                if(map == NULL)
                {
                    return NULL;
                }
                
                MapNode* head = malloc(sizeof(*head));
                if(head == NULL)
                {
                    return NULL;
                }
                head->key = NULL;
                head->value = NULL;

                //initialize iterator and the head of the LL
                map->iterator = map->elements = head;
                
                map->copyDataElement = copyDataElement;
                map->copyKeyElement = copyKeyElement;
                map->compareElements = compareKeyElements;
                map->freeDataElement = freeDataElement;
                map->freeKeyElement = freeKeyElement;
                
                return map;
}


void mapDestroy(Map map)
{
    MapNode *head = map->elements;
    map->elements = map->elements->next;

    while (map->elements != NULL)
    {
        map->freeKeyElement(map->elements->key);
        map->freeDataElement(map->elements->value);

        MapNode *tmp = map->elements->next;
        
        free(map->elements);
        
        map->elements = tmp;
    }
    free(head);
    free(map);
}

Map mapCopy(Map map)
{
    Map new_map = mapCreate(map->copyDataElement,
                            map->copyKeyElement,
                            map->freeDataElement,      
                            map->freeKeyElement,
                            map->compareElements);

    MapNode *current = map->elements;

    while (current != NULL)
    {
        if(mapPut(new_map, current->key, current->value) == MAP_NULL_ARGUMENT)
        {
            return NULL;
        }
        current = current->next;
    }   
    return new_map;
}


int mapGetSize(Map map)
{
    int counter = 0;
    MapNode *current = map->elements;

    while (current->next != NULL)
    {
        counter++;
        current = current->next;
    }   
    return counter;
}


bool mapContains(Map map, MapKeyElement element)
{
    //resets the internal iterator as requested...
    map->iterator = map->elements;

    MapNode *current = map->elements;

    while (current != NULL)
    {
       if(!map->compareElements(element, current->key))
       {
           return true;
       }
       current = current->next;
    }  
    return false; 
}


MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    map->iterator = map->elements;

    SearchResults results;
    MapNode* previous = findPreviousElementPosition(map, keyElement, &results);

    if(results == FOUND)
    {
        map->freeDataElement(previous->next->value);
        previous->next->value = map->copyDataElement(dataElement);
       
        return MAP_ITEM_ALREADY_EXISTS;
    }
    else
    {
        MapNode* new_node = malloc(sizeof(*new_node));
        if(new_node == NULL)
        {
            return MAP_OUT_OF_MEMORY;
        }
        new_node->value = map->copyDataElement(dataElement);
        new_node->key = map->copyKeyElement(keyElement);
        new_node->next = previous->next;

        previous->next = new_node;
    }

    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    MapNode *current = map->elements->next;

    while (current != NULL)
    {
       if(!map->compareElements(keyElement, current->key))
       {
           return current->value;
       }
       current = current->next;
    }
    //assuming the user supply a valid key i guess..
    return NULL;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    SearchResults result;
    MapNode* previous = findPreviousElementPosition(map, keyElement, &result);
    
    if (result == FOUND)
    {
        map->freeKeyElement(previous->next->key);
        map->freeDataElement(previous->next->value);

        MapNode* tmp = previous->next->next;
        
        free(previous->next);

        previous->next = tmp;

        return MAP_SUCCESS;
    }

    return MAP_ITEM_DOES_NOT_EXIST; 
}

MapKeyElement mapGetFirst(Map map)
{
    map->iterator = map->elements;
    return mapGetNext(map);
}

MapKeyElement mapGetNext(Map map)
{
    if(map->iterator->next == NULL)
    {
        return NULL;
    }
    MapKeyElement key = map->iterator->next->key;
   
    map->iterator = map->iterator->next;
    
    return key;
}

MapResult mapClear(Map map)
{
    while (map->elements != NULL)
    {
        map->freeKeyElement(map->elements->key);
        map->freeDataElement(map->elements->value);

        MapNode *tmp = map->elements->next;
        
        free(map->elements);
        
        map->elements = tmp;
        
    }
    return MAP_SUCCESS;
}

MapNode* findPreviousElementPosition(Map map, MapKeyElement key, SearchResults* searchResults)
{
    int result;
    MapNode *current = map->elements;

    while (current->next != NULL)
    {
        result = map->compareElements(key, current->next->key);

        if (result < 0) //Next is larger.
        {
            break;
        }
        else if (result == 0)
        {
            if (searchResults != NULL)
            {
                *searchResults = FOUND;
            }
        }
        else //Next is smaller.
        {
            current = current->next;
        }
    }

    //If we're here then all list elements were smaller.
    if (searchResults != NULL)
    {
        *searchResults = NOT_FOUND;
    }
    return current;
}
