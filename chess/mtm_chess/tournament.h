#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "map.h"
#include "game.h"
#include "chessSystem.h"
#include <stdio.h>
#include <stdbool.h>


#define LOCATION_MAX_LENGTH 100


typedef enum {
    NONE,
    INVALID_LOCATION,
    INVALID_MAX_GAMES,
    INVALID_ID,
    MEMORY_ALLOCATION
} TournamentErrors;


typedef struct Tournament_t *Tournament;

//Function declarations:
bool playedMaximumGames(Tournament *tournament, int player);
bool invalidLocation(char* tournament_location);
bool alreadyExistsInTournament(Map games, int first_player,int second_player);

//Construction & destruction:
Tournament createTournament(int tournament_id, TournamentErrors *error);
void freeTournament(Tournament tournament);

//Getters & setters:
int getId(Tournament tournament);
int calculateWinner(Tournament tournament);

//Remember to free the return value of the following getters (relevant for the user):
char* getLocation(Tournament tournament);
int getPlayerGameLimit(Tournament tournament);
int getPlayerCount(Tournament tournament);


#endif