
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

}
int mapGetSize(Map map)
{

}
bool mapContains(Map map, MapKeyElement element)
{

}
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    
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