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
ChessResult removePlayer(Tournament tournament, int player_id);

Tournament copyTournament(Tournament src);
ChessResult endTournament(Tournament tournament);
bool isFinished(Tournament tournament);
int compareTournamentKeys(int key1, int key2);
int getGameCount(Tournament tournament);
//If game_count is not null, the total game count will be put in it.
int getTotalPlayerPlayTime(Tournament tournament, int id, int* game_count);
void getGameTimeStatistics(Tournament tournament, int *longest_time, double *average_time);

//Remember to free the return value of the following getters (relevant for the user):
char* getLocation(Tournament tournament);
int getPlayerGameLimit(Tournament tournament);
int getPlayerCount(Tournament tournament);

//Map-related functions:
MapDataElement mapTournamentCopy(MapDataElement tournament);
MapKeyElement mapTournamentIdCopy(MapKeyElement id);
void mapTournamentDataFree(MapDataElement tournament);
void mapTournamentIdFree(MapKeyElement id);
int mapTournamentKeyCompare(MapKeyElement id1, MapKeyElement id2);

#endif