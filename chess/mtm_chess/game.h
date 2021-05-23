#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>
#include "tournament.h"
#include "chessSystem.h"
#include "map.h"



typedef struct Game_t *Game;

//Construction & destruction:
Game createGame(int id_player1, int id_player2, Winner winner, int game_time, ChessResult* error);

void freeGame(Game game);

//Getters & setters:
int getPlayer1Id(Game game);
int getPlayer2Id(Game game);
Winner getWinner(Game game);
int getWinnerId(Game game);
bool isPlayerForfeited(Game game);
void setPlayerForfeited(Game game, int player_to_remove_id);
int getTime(Game game);
Game copyGame(Game src);
void freeGameKey(int key);
int copyGameId(int id);
int compareGameKeys(int key1, int key2);

//This returns true if the player was one of the players, else false.
//! NOTE: If player played but was removed (i.e player lost in auto-win), the value will be false.
//(As per ourinstructions.)
bool didPlayerPlay(Game game, int player_id);

//Returns the length of the game if given player participated in it; else returns 0.
int getPlayerPlayTime(Game game, int player_id);

//Map-related functions:
MapDataElement mapGameCopy(MapDataElement game);
MapKeyElement mapGameIdCopy(MapKeyElement id);
void mapGameDataFree(MapDataElement game);
void mapGameIdFree(MapKeyElement gameId);
int mapGameKeyCompare(MapKeyElement id1, MapKeyElement id2);

#endif