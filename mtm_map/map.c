
#include "map.h"
#include <stdio.h>
#include <stdlib.h>



int main()
{

    
}

typedef enum FindResults
{ 
  FOUND_BEFORE,
  FOUND,
}Results;

typedef struct LinkedListNode
{
    MapDataElement value;
    MapKeyElement key;
    struct LinkedListNode *next;
} MapNode;

//functions declaration
Results findElementPosition(Map map, MapNode** resultPtr, MapKeyElement key);

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

    while (current != NULL)
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
    MapNode* tmp;

    Results position = findElementPosition(map, &tmp, keyElement);

    if(position == FOUND)
    {
        map->freeDataElement(tmp->value);
        
        tmp->value = map->copyDataElement(dataElement);
        return MAP_ITEM_ALREADY_EXISTS;
    }
    else
    {
        MapNode* new_node = malloc(sizeof(*new_node));
        if(new_node == NULL)
        {
            return MAP_NULL_ARGUMENT;
        }

        new_node->next = tmp->next;
       
        tmp->next = new_node;

        new_node->value = map->copyDataElement(dataElement);
        new_node->key = map->copyKeyElement(keyElement);
        
        //reset the internal iterator
        map->iterator = map->elements;

        return MAP_ITEM_DOES_NOT_EXIST;
    }
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    MapNode *current = map->elements;

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
    MapNode *current = map->elements;

    while (current != NULL)
    {
       if(!map->compareElements(keyElement, current->key))
       {
            map->freeKeyElement(current->key);
            map->freeDataElement(current->value);

            MapNode *tmp = current->next;
            free(current);
            current = tmp;
             
            //reset the internal iterator
            map->iterator = map->elements;

            return MAP_SUCCESS;
       }
       current = current->next;
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
        return MAP_SUCCESS;
    }
}

Results findElementPosition(Map map, MapNode** resultPtr, MapKeyElement key)
{
    int res =0;
    MapNode *current = map->elements;

    while (current != NULL)
    {
       res = map->compareElements(key, current->key);

       if(res > 0)
       {
           if(current->next == NULL || map->compareElements(key, current->next->key) < 0)
           {
               *resultPtr = current;
               return FOUND_BEFORE;
           }
       }
       else if(res==0)
       {
           *resultPtr = current;
           return FOUND;
       }
       else
       {
           *resultPtr = map->elements;
           return FOUND_BEFORE;
       }
        current = current->next;
    }
}
