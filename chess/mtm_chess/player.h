#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
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


#endif //PLAYER_H