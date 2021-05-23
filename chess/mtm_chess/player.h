#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include "map.h"
#define ILLEGAL_PLAYER (-1)

//Type to expose:
typedef struct Player_t *Player;

//Construction & destruction:
Player createPlayer(int player_id);
void freePlayer(Player player);
//Getters & setters:
int getWins(Player player);
void setWins(Player player, int wins);
void increaseWins(Player player);
int getDraws(Player player);
void setDraws(Player player, int draws);
void increaseDraws(Player player);
int getLosses(Player player);
void setLosses(Player player, int losses);
void increaseLosses(Player player);
Player copyPlayer(Player player);
int playerScore(Player player);
void freePlayerKey(int key);
int copyPlayerId(int id);
int comparePlayerKeys(int key1, int key2);

//Map-related functions:
MapDataElement mapPlayerCopy(MapDataElement player);
MapKeyElement mapPlayerIdCopy(MapKeyElement id);
void mapPlayerDataFree(MapDataElement player);
void mapPlayerIdFree(MapKeyElement id);
int mapPlayerKeyCompare(MapKeyElement id1, MapKeyElement id2);



#endif //PLAYER_H