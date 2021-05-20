#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdio.h>
#include "tournament.h"
#include "chessSystem.h"
#include "map.h"



typedef struct Game_t *Game;

//Construction & destruction:
Game createGame(int id_player1, int id_player2, Winner winner, double game_time);

void freeGame(Game game);

//Getters & setters:
int getPlayer1Id(Game game);
int getPlayer2Id(Game game);
Winner getWinner(Game game);
bool isPlayerForfeited(Game game);
void setPlayerForfeited(Game game, int player_to_remove);
double getTime(Game game);


#endif