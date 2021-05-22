#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "map.h"
#include "game.h"
#include "player.h"
#include "chessSystem.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define LOCATION_MAX_LENGTH 100
#define NO_WINNER -1


/*     
typedef enum {
    NONE,
    INVALID_LOCATION,
    INVALID_MAX_GAMES,
    INVALID_ID,
    MEMORY_ALLOCATION
} TournamentErrors;
*/


typedef struct Tournament_t *Tournament;

//Function declarations:
bool alreadyExistsInTournament(Tournament tournament, int first_player,int second_player);
int calculateTournamentWinner(Tournament tournament);
//Map getTournamentPlayers(Tournament tournament); //TODO: Replace with appropriate methods.

//Construction & destruction:
Tournament createTournament(int tournament_id, const char* location_str,
                            int max_games_per_player, ChessResult* error);
void freeTournament(Tournament tournament);

//Getters & setters:
int getId(Tournament tournament);
int getTournamentWinner(Tournament tournament);
//Map getGames(Tournament tournament); //TODO: Remove.
ChessResult addGameToTournament(Tournament tournament, int first_player, int second_player, Winner winner, int play_time, Map players_map); //TODO: Implement.
Tournament copyTournament(Tournament src);
void setTournamentWinner(Tournament tournament, int winner);
void close(Tournament tournament);
bool isClosed(Tournament tournament);
void freeTournamentKey(int key);
int copyTournamentId(int id);
int compareTournamentKeys(int key1, int key2);

//Remember to free the return value of the following getters (relevant for the user):
char* getLocation(Tournament tournament);
int getPlayerGameLimit(Tournament tournament);
int getPlayerCount(Tournament tournament);


#endif