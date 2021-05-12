#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "chessSystem.h"
#include "map.h"

//TODO: Check if we're supposed to specify a constant length or not.
#define LOCATION_MAX_LENGTH 1000

struct chess_system_t
{
    Map tournaments;      
    Map games;      
};

typedef struct game_t
{
    unsigned int game_id;
    unsigned int tournament_id;
    int id_player1;
    int id_player2;
    Winner score;
    int game_time;

} Game;


typedef struct tournament_t
{
    unsigned int tournament_id;
    char* location;
    Map game_ptrs;
    int winner;

} Tournament;

//functions declaration
bool invalidLocation(char* tournament_location);
int gameId(Map games);


//methods declaration
static MapDataElement copyTournamentData(MapDataElement tournament);
static MapKeyElement copyIntegerId(MapKeyElement key);
static void freeIntegerKey(MapKeyElement key);
static void freeTournamentData(MapDataElement tournament);
static int compareIntegerKeys(MapKeyElement key1, MapKeyElement key2);
static void freeGameData(MapDataElement game);
static MapDataElement copyGameData(MapDataElement game);


//Tournament and Game map methods:
static MapDataElement copyTournamentData(MapDataElement tournament)
{
    if (tournament == NULL)
    {
        return NULL;
    }
    Tournament *copy = malloc(sizeof(*copy));
    if (copy == NULL)
    {
        return NULL;
    }

    Tournament src = *((Tournament*)tournament);
    copy->tournament_id = src.tournament_id;
    copy->location = malloc(sizeof(*(copy->location)) * LOCATION_MAX_LENGTH);
    strcpy(copy->location, src.location);
    copy->game_ptrs = mapCopy(src.game_ptrs);
    copy->winner = src.winner;

    return copy;
}

static MapKeyElement copyIntegerId(MapKeyElement key)
{
    if (!key) 
    {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    
    if (!copy)
    {
        return NULL;
    }
    
    *copy = *(int *) key;
    return copy;
}

static void freeIntegerKey(MapKeyElement key)
{
    //Our key is an integer so nothing needs to be done.
}

static void freeTournamentData(MapDataElement tournament)
{
    if(!tournament)
    {
        return NULL;
    }
    Tournament tournamentToFree = *(Tournament *) tournament;
    
    free(tournamentToFree.location);
    mapDestroy(tournamentToFree.game_ptrs);
    free(tournament);
    
    return;
}

static int compareIntegerKeys(MapKeyElement key1, MapKeyElement key2) 
{
    return (*(int *) key1 - *(int *) key2);
}

static void freeGameData(MapDataElement game)
{
    free(game);
    return;
}

static MapDataElement copyGameData(MapDataElement game)
{
    if(!game)
    {
        return NULL;
    }
    Game *copy = malloc(sizeof(*copy));
    if(copy==NULL)
    {
        return NULL;
    }

    Game src = *((Game *) game);

    copy->game_id = src.game_id;
    copy->game_time = src.game_time;
    copy->id_player1 = src.id_player1;
    copy->id_player2 = src.id_player2;
    copy->score = src.score;
    copy->tournament_id = src.tournament_id;

    return copy;
}

ChessSystem chessCreate()                 
{   
    Map tournaments = mapCreate(&copyTournamentData,
                                &copyIntegerId,
                                &freeTournamentData,
                                &freeIntegerKey,
                                &compareIntegerKeys);
    Map games = mapCreate(&copyGameData,
                        &copyIntegerId,
                        &freeGameData,
                        &freeIntegerKey,
                        &compareIntegerKeys);

    ChessSystem chess_system = malloc(sizeof(*chess_system));
    if(chess_system == NULL)
    {
        return NULL;
    }
    
    return chess_system;        
}

void chessDestroy(ChessSystem chess)
{
    if(chess == NULL)
    {
        return;
    }

    mapDestroy(chess->tournaments);
    mapDestroy(chess->games);
    free(chess);
    return;
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    if(invalidLocation(tournament_location))
    {
        return CHESS_INVALID_LOCATION;
    }
    if(max_games_per_player <= 0)
    {
        return CHESS_INVALID_MAX_GAMES;
    }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID; 
    }

    Tournament *tournament = malloc(sizeof(*tournament));
    if(tournament == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    MapResult result = mapPut(chess->tournaments, tournament_id, tournament);
    
    if(result == MAP_ITEM_ALREADY_EXISTS)
    {   
        free(tournament);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    else
    {
        return CHESS_SUCCESS;
    }
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }
    else if(tournament_id <= 0 || first_player <= 0 || second_player <= 0)
    {
        return CHESS_INVALID_ID;
    }
    else if(!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    else if(chessEndTournament(chess, tournament_id) == CHESS_TOURNAMENT_ENDED)
    {
        return CHESS_TOURNAMENT_ENDED;
    }

    Game* gameToAdd = malloc(sizeof(*gameToAdd));
    
    gameToAdd->tournament_id = tournament_id;
    gameToAdd->id_player1 = first_player;
    gameToAdd->id_player2 = second_player;
    gameToAdd->score = winner;
    gameToAdd->game_time = play_time;

    

    MapResult result = mapPut(chess->games, gameToAdd->game_id, gameToAdd);

    //set game id to be the size of the games map (there is no removals of games from the map so it's works)
    gameToAdd->game_id = gameId(chess->games);

    if(result == MAP_ITEM_ALREADY_EXISTS)
    {   
        free(gameToAdd);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    else
    {
        return CHESS_SUCCESS;
    }
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
	
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
	
}

ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
	
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
	
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
	
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
	
}

bool invalidLocation(char* tournament_location)
{
    int i = 1;
    if(tournament_location[0] < 'Z' && tournament_location[0] > 'A')
    {
        while(tournament_location[i] != NULL)
        {
            if((tournament_location[i] < 'z' && tournament_location[i] > 'a') || tournament_location[i] == ' ')
            {
                i++;
            }
            else
            {
                return true;
            }
        }  
        return false;
    }
    else
    {
        return true;
    }    
}

int gameId(Map games)
{
    return mapGetSize(games);
}

 
