#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <assert.h>
#include "map.h"
#define ILLEGAL_PLAYER (-1)

//Type to expose:
typedef struct Player_t *Player;

//Construction & destruction:
Player createPlayer(int player_id);
void freePlayer(Player player);

//Getters:
int getWins(Player player);
int getLosses(Player player);
int getDraws(Player player);

//Additional functions:
void increaseWins(Player player);
void increaseLosses(Player player);
void increaseDraws(Player player);
void decreaseWins(Player player);
void decreaseLosses(Player player);
void decreaseDraws(Player player);
Player copyPlayer(Player player);

//Map-related functions:
MapDataElement mapPlayerCopy(MapDataElement player);
MapKeyElement mapPlayerIdCopy(MapKeyElement id);
void mapPlayerDataFree(MapDataElement player);
void mapPlayerIdFree(MapKeyElement id);
int mapPlayerKeyCompare(MapKeyElement id1, MapKeyElement id2);


#endif //PLAYER_H