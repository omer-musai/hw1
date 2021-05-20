#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "chessSystem.h"
#include "map.h"
#include "player.h"
#include "tournament.h"
#include "game.h"
/*
           ___
         .';:;'.
        /_' _' /\   __
        ;a/ e= J/-'"  '.
        \ ~_   (  -'  ( ;_ ,.
         L~"'_.    -.  \ ./  )
         ,'-' '-._  _;  )'   (
       .' .'   _.'")  \  \(  |
      /  (  .-'   __\{`', \  |
     / .'  /  _.-'   "  ; /  |
    / /    '-._'-,     / / \ (
 __/ (_    ,;' .-'    / /  /_'-._
`"-'` ~`  ccc.'   __.','     \j\L\
                 .='/|\7      
                      ' `
*/

//TODO: Check if we're supposed to specify a constant length or not.
#define LOCATION_MAX_LENGTH 1000
#define NO_WINNER -1

struct chess_system_t
{
    Map players;
    Map tournaments;           
};


//functions declaration
bool alreadyExistsInSystem(ChessSystem chess, int first_player, int second_player);

//methods declaration
static MapDataElement copyTournamentData(MapDataElement tournament);
static MapKeyElement copyIntegerId(MapKeyElement key);
static void freeIntegerKey(MapKeyElement key);
static void freeTournamentData(MapDataElement tournament);
static int compareIntegerKeys(MapKeyElement key1, MapKeyElement key2);
static void freeGameData(MapDataElement game);
static MapDataElement copyGameData(MapDataElement game);
static MapDataElement copyPlayerData(MapDataElement player);
static void freePlayerData(MapDataElement player);


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
    copy->player_count = src.player_count;
    copy->location = malloc(sizeof(*(copy->location)) * LOCATION_MAX_LENGTH);
    strcpy(copy->location, src.location);
    copy->games = mapCopy(src.games);
    copy->winner = src.winner;
    copy->max_games_per_player = src.max_games_per_player;

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
    mapDestroy(tournamentToFree.games);
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

    copy->game_time = src.game_time;
    copy->id_player1 = src.id_player1;
    copy->id_player2 = src.id_player2;
    copy->winner = src.winner;
    copy->auto_win = src.auto_win;

    return copy;
}

static MapDataElement copyPlayerData(MapDataElement player)
{
    if(!player)
    {
        return NULL;
    }
    Game *copy = malloc(sizeof(*copy));
    if(copy==NULL)
    {
        return NULL;
    }
}

static void freePlayerData(MapDataElement player)
{
    free(player);
    return;
}



ChessSystem chessCreate()                 
{   
    Map tournaments = mapCreate(&copyTournamentData,
                                &copyIntegerId,
                                &freeTournamentData,
                                &freeIntegerKey,
                                &compareIntegerKeys);


    Map players = mapCreate(&copyPlayerData,
                                &copyIntegerId,
                                &freePlayerData,
                                &freeIntegerKey,
                                &compareIntegerKeys);

    ChessSystem chess_system = malloc(sizeof(*chess_system));
    if(chess_system == NULL)
    {
        return  CHESS_OUT_OF_MEMORY;
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
    mapDestroy(chess->players);
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
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    MapResult result = mapPut(chess->tournaments, tournament_id, tournament);
    
    if(result == MAP_ITEM_ALREADY_EXISTS)
    {   
        free(tournament);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    
     
    tournament->location = malloc(sizeof(*(tournament->location)) * LOCATION_MAX_LENGTH);
    if (tournament->location == NULL)
    {
        free(tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    strcpy(tournament->location, tournament_location);

    tournament->games = mapCreate(&copyGameData,
                        &copyIntegerId,
                        &freeGameData,
                        &freeIntegerKey,
                        &compareIntegerKeys);
    if (tournament->games == NULL)
    {
        free(tournament->location);
        free(tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    tournament->max_games_per_player = max_games_per_player;
    tournament->player_count = 0;
    tournament->tournament_id = tournament_id; 

    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    Tournament *tournament = mapGet(chess->tournaments, tournament_id);
    

    if(play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }
    else if(tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }
    else if(tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    else if(winner != NO_WINNER)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if(alreadyExistsInSystem(chess, first_player, second_player))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    
    if(playedMaximumGames(tournament, first_player) || playedMaximumGames(tournament, second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }

    Game* gameToAdd = malloc(sizeof(*gameToAdd));
    if(gameToAdd == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    
    gameToAdd->id_player1 = first_player;
    gameToAdd->id_player2 = second_player;
    gameToAdd->winner = winner;
    gameToAdd->game_time = play_time;
    gameToAdd->auto_win = false;
    
    MapResult result = mapPut(tournament->games, mapGetSize(tournament->games), gameToAdd);

    if (result == MAP_OUT_OF_MEMORY || result == MAP_NULL_ARGUMENT)
    {
        return NULL;
    }
    
    return CHESS_SUCCESS;
}

MapResult addGameToTournament(ChessSystem chess, Game* game, int tournament_id)
{
    /*
        TODO:
        Add to tournament.
        Increment player count if needed.
        Add to players map if that's player's first game; otherwise, update.
    */
    Tournament* tournament = mapGet(chess->tournaments, &tournament_id);

    MAP_FOREACH(int, current_game, tournament->games)
    {
        if ()
    }
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{   
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    
    MapDataElement tmp = mapGet(chess->tournaments, tournament_id);

    Tournament current_tournament = *(Tournament *) tmp;
    
    mapRemove(chess->tournaments, tournament_id);

    //TODO: need to update the game statistics!
	
    return CHESS_SUCCESS;
}

//TODO: in process
ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
	if(player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    Tournament *tournament;
    Game *game;

    MAP_FOREACH(int, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, &current_tournament);
       
        MAP_FOREACH(int, current_game, tournament->games )
        {
            game = mapGet(tournament->games, &current_game);

            if(game->id_player1 == player_id)
            {
               game->winner = SECOND_PLAYER;
               game->auto_win = true;
            }
            if(game->id_player2 == player_id)
            {
                game->winner = FIRST_PLAYER;
                game->auto_win = true;
            }
        }
    }
    
    MapResult tmp = mapRemove(chess->players, player_id);
    if(tmp != MAP_SUCCESS)
    {
      return MAP_OUT_OF_MEMORY;
    }
    
    return CHESS_SUCCESS;
}


ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    Tournament *tournament;

    tournament = mapGet(chess->tournaments, tournament_id);
    

	
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    double total_time;
    int num_of_games =0; 

    if(player_id < 0)
    {
        chess_result =  CHESS_INVALID_ID;
        return 0;
    }

    Tournament tournament;
    Game game;

    MAP_FOREACH(int, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, &current_tournament);
       
        MAP_FOREACH(int, current_game, tournament->games )
        {
            game = mapGet(tournament->games, &current_game);

            if(game->id_player1 == player_id || game->id_player2 == player_id)
            {
                total_time += game->game_time;
                num_of_games++;
            }
        }
    }
     
    if(num_of_games == 0)
    {
        chess_result == CHESS_PLAYER_NOT_EXIST;
        return 0;
    }

    double avg_time = (total_time / num_of_games);
  
    return avg_time;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
	
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
	
}


bool alreadyExistsInSystem(ChessSystem chess, int first_player, int second_player)
{
    Tournament current_tournament;
    MAP_FOREACH(int, tournament_id, chess->tournaments)
    {
        current_tournament = mapGet(chess->tournaments, &tournament_id);
        if (alreadyExistsInTournament(current_tournament->games, first_player, second_player))
        {
            return true;
        }
    }
    return false;
}

 
