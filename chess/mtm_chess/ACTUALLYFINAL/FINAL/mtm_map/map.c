#include "map.h"
#include <stdlib.h>
#include <assert.h>

#define INVALID -1

typedef struct LinkedListNode
{
    MapDataElement value;
    MapKeyElement key;
    struct LinkedListNode *next;
} *MapNode;

typedef enum SearchResults {
    FOUND,
    NOT_FOUND,
    FIRST_ELEMENT,
    NEEDS_TO_BE_FIRST
} SearchResults;

struct Map_t
{
    MapNode iterator;
    MapNode elements;

    int length;

    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    compareMapKeyElements compareElements;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
};

//Declaring static auxiliary functions:
static MapNode findPreviousElementPosition(Map map, MapKeyElement key, SearchResults* results);
static MapNode copyList(Map map, MapNode src);
static void freeList(Map map, MapNode list);
//addOrUpdateNode is designed to work with findPreviousElementPosition.
//The position_node parameter is the return value of findPreviousElementPosition.
//This function also updates the length of the map if necessary.
static MapResult addOrUpdateNode(Map map, MapDataElement data_copy, MapKeyElement key_copy, MapNode position_node, SearchResults results);

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if (copyDataElement == NULL || copyKeyElement == NULL || freeDataElement == NULL
        || freeKeyElement == NULL || compareKeyElements == NULL)
    {
        return NULL;
    }

    Map map = malloc(sizeof(*map));
    if (map == NULL)
    {
        return NULL;
    }

    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyKeyElement;
    map->freeDataElement = freeDataElement;
    map->freeKeyElement = freeKeyElement;
    map->compareElements = compareKeyElements;

    map->elements = NULL;
    map->iterator = NULL;
    map->length = 0;

    return map;
}

void mapDestroy(Map map)
{
    if (map == NULL)
    {
        return;
    }

    freeList(map, map->elements);

    free(map);
}

Map mapCopy(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }

    Map new_map = malloc(sizeof(*new_map));
    if (new_map == NULL)
    {
        return NULL;
    }

    new_map->length = map->length;
    new_map->copyDataElement = map->copyDataElement;
    new_map->copyKeyElement = map->copyKeyElement;
    new_map->freeDataElement = map->freeDataElement;
    new_map->freeKeyElement = map->freeKeyElement;
    new_map->compareElements = map->compareElements;

    new_map->elements = copyList(map, map->elements);
    new_map->iterator = new_map->elements;

    return new_map;
}

int mapGetSize(Map map)
{
    if (map == NULL)
    {
        return INVALID;
    }
    return map->length;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (map == NULL || element == NULL)
    {
        return false;
    }

    SearchResults results;
    findPreviousElementPosition(map, element, &results);
    return (results == FOUND || results == FIRST_ELEMENT);
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (map == NULL || keyElement == NULL || dataElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    MapDataElement data_copy = map->copyDataElement(dataElement);
    if (data_copy == NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }
    MapKeyElement key_copy = map->copyKeyElement(keyElement);
    if (key_copy == NULL)
    {
        free(data_copy);
        return MAP_OUT_OF_MEMORY;
    }

    SearchResults results;
    MapNode node = findPreviousElementPosition(map, keyElement, &results);

    MapResult error = addOrUpdateNode(map, data_copy, key_copy, node, results);

    if (error == MAP_OUT_OF_MEMORY)
    {
        free(data_copy);
        free(key_copy);
    }

    return error;
}



MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return NULL;
    }

    SearchResults results;
    MapNode position = findPreviousElementPosition(map, keyElement, &results);

    if (results == NOT_FOUND || results == NEEDS_TO_BE_FIRST)
    {
        return NULL;
    }

    if (results == FIRST_ELEMENT)
    {
        return map->elements->value;
    }


    //Else: results == FOUND
    return position->next->value;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if (map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    SearchResults results;
    MapNode previous = findPreviousElementPosition(map, keyElement, &results);
    if (results == NOT_FOUND || results == NEEDS_TO_BE_FIRST)
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    if (results == FIRST_ELEMENT)
    {
        map->freeDataElement(map->elements->value);
        map->freeKeyElement(map->elements->key);
        MapNode new_head = map->elements->next;
        free(map->elements);
        map->elements = new_head;
    }
    else //results == FOUND
    {
        assert(previous != NULL);
        map->freeKeyElement(previous->next->key);
        map->freeDataElement(previous->next->value);
        MapNode new_next = previous->next->next;
        free(previous->next);
        previous->next = new_next;
    }
    --(map->length);
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map)
{
    if (map == NULL)
    {
        return NULL;
    }

    map->iterator = map->elements;
    return mapGetNext(map);
}

MapKeyElement mapGetNext(Map map)
{
    if (map == NULL || map->iterator == NULL)
    {
        return NULL;
    }

    MapKeyElement copy = map->copyKeyElement(map->iterator->key);
    if (copy == NULL)
    {
        return NULL;
    }

    map->iterator = map->iterator->next;

    return copy;
}

MapResult mapClear(Map map)
{
    if (map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    freeList(map, map->elements);
    map->elements = NULL;
    map->length = 0;

    return MAP_SUCCESS;
}


//Static auxiliary functions:
static MapNode findPreviousElementPosition(Map map, MapKeyElement key, SearchResults* results)
{
    if (map == NULL || key == NULL || results == NULL)
    {
        return NULL;
    }

    *results = NOT_FOUND;
    MapNode current = map->elements, next_node, previousElement = NULL;
    int comparison;

    if (current != NULL)
    {
        comparison = map->compareElements(current->key, key);
        if (comparison > 0)
        {
            *results = NEEDS_TO_BE_FIRST;
            return NULL;
        }
        if (comparison == 0)
        {
            *results = FIRST_ELEMENT;
            return NULL;
        }

        while (current != NULL)
        {
            next_node = current->next;
            if (next_node == NULL || (comparison = map->compareElements(next_node->key, key)) > 0)
            {
                *results = NOT_FOUND;
                previousElement = current;
                break;
            }
            if (comparison == 0)
            {
                *results = FOUND;
                previousElement = current;
                break;
            }
            current = current->next;
        }
    }
    else
    {
        //Empty list.
        *results = NEEDS_TO_BE_FIRST;
    }

    return previousElement;
}

static MapNode copyList(Map map, MapNode src)
{
    MapNode dest = NULL, last = NULL;

    while (src != NULL)
    {
        MapNode new_node = malloc(sizeof(*new_node));
        if (new_node == NULL)
        {
            freeList(map, dest);
            return NULL;
        }

        new_node->next = NULL;

        new_node->value = map->copyDataElement(src->value);
        if (new_node->value == NULL)
        {
            freeList(map, dest);
            return NULL;
        }
        new_node->key = map->copyKeyElement(src->key);
        if (new_node->key == NULL)
        {
            free(new_node->value);
            freeList(map, dest);
            return NULL;
        }
        if (dest == NULL)
        {
            dest = new_node;
            last = dest;
        }
        else
        {
            last->next = new_node;
            last = new_node;
        }

        src = src->next;
    }

    return dest;
}

static void freeList(Map map, MapNode list)
{
    if (map == NULL)
    {
        return;
    }

    while (list != NULL)
    {
        MapNode next_node = list->next;
        map->freeDataElement(list->value);
        map->freeKeyElement(list->key);
        free(list);
        list = next_node;
    }
}

static MapResult addOrUpdateNode(Map map, MapDataElement data_copy, MapKeyElement key_copy, MapNode position_node, SearchResults results)
{
    if (results == FOUND || results == FIRST_ELEMENT)
    {
        if (results == FIRST_ELEMENT)
        {
            position_node = map->elements;
        }
        else
        {
            assert(position_node != NULL);
            position_node = position_node->next;
        }
        map->freeDataElement(position_node->value);
        map->freeKeyElement(position_node->key);
        position_node->value = data_copy;
        position_node->key = key_copy;

        return MAP_SUCCESS;
    }

    MapNode new_node = malloc(sizeof(*new_node));
    if (new_node == NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }
    new_node->value = data_copy;
    new_node->key = key_copy;
    new_node->next = NULL;

    if (results == NEEDS_TO_BE_FIRST)
    {
        new_node->next = map->elements;
        map->elements = new_node;
    }
    else //NOT_FOUND
    {
        assert(position_node != NULL);
        new_node->next = position_node->next;
        position_node->next = new_node;
    }

    ++(map->length);
    return MAP_SUCCESS;
}
