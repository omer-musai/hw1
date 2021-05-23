#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "map.h"
#include "game.h"
#include "player.h"
#include "chessSystem.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define NO_WINNER -1

typedef struct Tournament_t *Tournament;



//Construction & destruction:
Tournament createTournament(int tournament_id, const char* location_str,
                            int max_games_per_player, ChessResult* error);

void freeTournament(Tournament tournament);

//Getters & setters:
int getId(Tournament tournament);
int getTournamentWinner(Tournament tournament);
int getGameCount(Tournament tournament);
//If game_count is not null, the total game count will be put in it.
int getTotalPlayerPlayTime(Tournament tournament, int id, int* game_count);
void getGameTimeStatistics(Tournament tournament, int *longest_time, double *average_time);
bool isFinished(Tournament tournament);
char* getLocation(Tournament tournament);
int getPlayerGameLimit(Tournament tournament);
int getPlayerCount(Tournament tournament);

//additional functions:
ChessResult addGameToTournament(Tournament tournament, int first_player, int second_player,
                                Winner winner, int play_time, Map players_map);

//This updates player statistics before a tournament's removal.
//The players map is the one stored in the ChessSystem.
ChessResult removeTournamentFromStatistics(Tournament tournament, Map players);
ChessResult decreaseTournamentStatistics(Game game, Player player1, Player player2);
ChessResult removePlayer(Tournament tournament, int player_id);
bool alreadyExistsInTournament(Tournament tournament, int first_player,int second_player);
int calculateTournamentWinner(Tournament tournament);
Map intTournamentPlayersMap(Map players_in_tournament, Tournament tournament);
Map createTournamentPlayersMap(Tournament tournament);
Tournament copyTournament(Tournament src);
ChessResult endTournament(Tournament tournament);

//Map-related functions:
MapDataElement mapTournamentCopy(MapDataElement tournament);
MapKeyElement mapTournamentIdCopy(MapKeyElement id);
void mapTournamentDataFree(MapDataElement tournament);
void mapTournamentIdFree(MapKeyElement id);
int mapTournamentKeyCompare(MapKeyElement id1, MapKeyElement id2);

#endif