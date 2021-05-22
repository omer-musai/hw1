#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    Tournament copy = copyTournament((Tournament)tournament);
    
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
    freeTournament((Tournament)tournament);
    return;
}

static int compareIntegerKeys(MapKeyElement key1, MapKeyElement key2) 
{
    return (*(int *) key1 - *(int *) key2);
}


static void freeGameData(MapDataElement game)
{
    freeGame((Game)game);
    return;
}

static MapDataElement copyGameData(MapDataElement game)
{
    Game copy = copyGame((Game)game);
    return copy;
}

static MapDataElement copyPlayerData(MapDataElement player)
{
    Player copy = copyPlayer((Player)player);
    return copy;
}

static void freePlayerData(MapDataElement player)
{
    freePlayer(player);
    return;
}


ChessSystem chessCreate()                 
{   
    Map tournaments = mapCreate(&copyTournamentData,
                                &copyIntegerId,
                                &freeTournamentData,
                                &freeIntegerKey,
                                &compareIntegerKeys);
    
    if(tournaments == NULL)
    {
        return NULL;
    }

    Map players = mapCreate(&copyPlayerData,
                                &copyIntegerId,
                                &freePlayerData,
                                &freeIntegerKey,
                                &compareIntegerKeys);

    if(players == NULL)
    {
        mapDestroy(tournaments);
        return NULL;
    }

    ChessSystem chess_system = malloc(sizeof(*chess_system));
    if(chess_system == NULL)
    {
        mapDestroy(tournaments);
        mapDestroy(players);
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
    mapDestroy(chess->players);
    free(chess);
    return;
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    ChessResult* error;

    Tournament tournament = createTournament(tournament_id, tournament_location,
                            max_games_per_player, error);

    if(tournament == NULL)
    {
        if(*error == CHESS_OUT_OF_MEMORY)
        {
            chessDestrory(chess);
        }
        return *error;
    }

    MapResult result = mapPut(chess->tournaments, tournament_id, tournament);
    
    if(result == MAP_ITEM_ALREADY_EXISTS)
    {   
        freeTournament(tournament);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if(result == MAP_OUT_OF_MEMORY)
    {
        freeTournament(tournament);
        chessDestrory(chess);
        return MAP_OUT_OF_MEMORY;
    }
    
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    ChessResult* error;
    Tournament tournament = mapGet(chess->tournaments, tournament_id);
    if(tournament == NULL)
    {
        return  CHESS_TOURNAMENT_NOT_EXIST;
    }

    if(alreadyExistsInSystem(chess, first_player, second_player))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(playedMaximumGames(tournament, first_player) || playedMaximumGames(tournament, second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }

    
    if(isClosed(tournament))
    {
        return CHESS_TOURNAMENT_ENDED;
    }

    Game game = createGame(first_player, second_player, winner, play_time, error);
    if(game == NULL)
    {
        if(*error == CHESS_OUT_OF_MEMORY)
        {
            chessDestrory(chess);
        }
        return *error;
    }

    Map games = getGames(tournament);

    //set key to be the size of the map
    MapResult result = mapPut(games, mapGetSize(games) + 1, game);

    if(result == MAP_OUT_OF_MEMORY)
    {
        freeGame(game);
        chessDestrory(chess);
        return MAP_OUT_OF_MEMORY;
    }
    
    return CHESS_SUCCESS;
}



ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{   
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament current_tournament = mapGet(chess->tournaments, tournament_id);
    if(current_tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    
    freeTournament(current_tournament);
    ChessResult result = mapRemove(chess->tournaments, tournament_id);
    
    

    //TODO: need to update the game statistics!
	
    return CHESS_SUCCESS;
}


ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
	if(player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    Player player = mapGet(chess->players, player_id);
    if(player == NULL)
    {
        return CHESS_PLAYER_NOT_EXIST;
    }

    Tournament tournament;
    Game game;

    MAP_FOREACH(int, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, &current_tournament);
       
        MAP_FOREACH(int, current_game, getGames(tournament) )
        {
            game = mapGet(getGames(tournament), &current_game);

            if(getPlayer1Id(game) == player_id)
            {
               setPlayerForfeited(game, FIRST_PLAYER);
            }
            if(getPlayer2Id(game) == player_id)
            {
                setPlayerForfeited(game, SECOND_PLAYER);
            }
        }
    }

    freePlayer(player);

    ChessResult result = mapRemove(chess->players, player_id);
    
    return CHESS_SUCCESS;
}


ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament tournament = mapGet(chess->tournaments, tournament_id);
    
    if(isClosed(tournament))
    {
        return  CHESS_TOURNAMENT_ENDED;
    }
    if(mapGetSize(getGames(tournament)) == 0)
    {
        return CHESS_NO_GAMES;   
    }

    int tournament_winner = calculateTournamentWinner(tournament);
    if(tournament_winner == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }

    setTournamentWinner(tournament, tournament_winner);
    close(tournament);

    return CHESS_SUCCESS;  

	
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
       
        MAP_FOREACH(int, current_game, getGames(tournament))
        {
            game = mapGet(getGames(tournament), &current_game);

            if(getPlayer1Id(game) == player_id || getPlayer2Id(game) == player_id)
            {
                total_time += getTime(game);
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
    double player_level;
    Player current_player;
    int wins;
    int losses;
    int draws;
    int counter =0;

    int* players_level = malloc(sizeof(int) * mapGetSize(chess->players));
    if(players_level == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    int* ids = malloc(sizeof(int) * mapGetSize(chess->players));
    if(ids == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }

	MAP_FOREACH(int, current_player_id, chess->players)
        {
            current_player = mapGet(chess->players, &current_player_id);
            wins = getWins(current_player);
            losses = getLosses(current_player);
            draws = getDraws(current_player);

            player_level = ((6*wins - 10*losses + 2*draws) / (wins + losses + draws));

            players_level[counter] = player_level;
            ids[counter] = current_player_id;

            //TODO sort the arrays 
        }

        
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
        if (alreadyExistsInTournament(getGames(current_tournament), first_player, second_player))
        {
            return true;
        }
    }
    return false;
}












/*
MapResult addGameToTournament(ChessSystem chess, Game* game, int tournament_id)
{
   
        TODO:
        Add to tournament.
        Increment player count if needed.
        Add to players map if that's player's first game; otherwise, update.
    
    Tournament* tournament = mapGet(chess->tournaments, &tournament_id);

    MAP_FOREACH(int, current_game, tournament->games)
    {
        if ()
    }
}
*/

 
