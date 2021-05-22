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
bool isPlayerForfeited(Game game);
void setPlayerForfeited(Game game, Winner player_to_remove);
int getTime(Game game);
Game copyGame(Game src);
void freeGameKey(int key);
int copyGameId(int id);
int compareGameKeys(int key1, int key2);



#endif