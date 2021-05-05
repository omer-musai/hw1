
#include "map.h"
#include <stdio.h>
#include <stdlib.h>


//TODO: Implement linked list.

typedef struct LinkedListNode
{
    MapDataElement value;
    MapKeyElement key;
    struct LinkedListNode *next;
} MapNode;

struct Map_t
{
    MapDataElement iterator;
    MapDataElement length; //Current length.
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
                map->elements = NULL;

                map->copyDataElement = copyDataElement;
                map->copyKeyElement = copyKeyElement;
                map->compareElements = compareKeyElements;
                map->freeDataElement = freeDataElement;
                map->freeKeyElement = freeKeyElement;
                
                return map;
}


void mapDestroy(Map map)
{

    while (map->elements != NULL)
    {
        map->freeKeyElement(map->elements->key);
        map->freeDataElement(map->elements->value);

        MapNode *tmp = map->elements->next;
        
        free(map->elements);
        
        map->elements = tmp;
    }
    
    free(map);
}

Map mapCopy(Map map)
{
    Map new_map = mapCreate(map->copyDataElement,
                            map->copyKeyElement,
                            map->freeDataElement,      
                            map->freeKeyElement,
                            map->compareElements);

    MapNode *temp = map->elements;

    while (temp != NULL)
    {
        if(mapPut(new_map, temp->key, temp->value) == MAP_NULL_ARGUMENT)
        {
            return NULL;
        }
        temp = temp->next;
    }   

    return new_map;
}


int mapGetSize(Map map)
{
    int counter = 0;
    MapNode *temp = map->elements;

    while (temp != NULL)
    {
        counter++;
        temp = temp->next;
    }   

    return counter;
}

//COMMUNICATION: in map.h it says "This resets the internal iterator", i didn't fully understood what should i do about it.
bool mapContains(Map map, MapKeyElement element)
{
    MapNode *temp = map->elements;

    while (temp != NULL)
    {
       if(element == temp->key)
       {
           return true;
       }
       temp = temp->next;
    }  

    return false; 
}

//COMMUNICATION: in process
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    MapNode *current = map->elements;

    //COMMUNICATION: didn't used the mapContains function because i need the location of the node anyway to change his value.
    while (current != NULL)
    {
       if(keyElement == current->key)
       {
           current->value = dataElement;
           return MAP_ITEM_ALREADY_EXISTS;
       }
       current = current->next;
    }

    current = malloc(sizeof(*current));
    if (current == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    //COMMUNICATION: not sure if that how it should be done ><
    current->key = copyKeyElement(keyElement);
    current->value = copyDataElement(dataElement);
    
    return MAP_SUCCESS;
}


MapDataElement mapGet(Map map, MapKeyElement keyElement)
{

}
MapResult mapRemove(Map map, MapKeyElement keyElement)
{

}
MapKeyElement mapGetFirst(Map map)
{

}
MapKeyElement mapGetNext(Map map)
{

}
MapResult mapClear(Map map)
{

}