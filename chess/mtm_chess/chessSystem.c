#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chessSystem.h"
#include "map.h"
#include "player.h"
#include "tournament.h"
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

//Implementing quicksort for the player level file saving:
static void swapIntegers(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
static void swapDoubles(double *a, double *b)
{
    double temp = *a;
    *a = *b;
    *b = temp;
}
static int partition(int *ids, double *levels, int length)
{
    int left = 1, right = length - 1;
    double pivot = levels[length/2];
    swapIntegers(ids, ids + length/2);
    swapDoubles(levels, levels + length/2);

    while (left <= right)
    {
        while (levels[left] < pivot && left < right)
        {
            ++left;
        }

        while (levels[right] >= pivot && left < right)
        {
            --right;
        }

        if (left == right)
        {
            if (levels[left] < pivot)
            {
                swapIntegers(ids + left, ids);
                swapDoubles(levels + left, levels);
                return left;
            }
            swapIntegers(ids + left - 1, ids);
            swapDoubles(levels + left - 1, levels);
            return left - 1;
        }
        swapIntegers(ids + left, ids + right);
        swapDoubles(levels + left, levels + right);
        ++left;
        --right;
    }
    return -1; //Shouldn't get here.
}
static void quicksort(int *ids, double *levels, int length)
{
    if (length <= 1)
    {
        return;
    }

    int pivot_index = partition(ids, levels, length);

    quicksort(ids, levels, pivot_index);
    quicksort(ids + pivot_index + 1, levels + pivot_index + 1, length - (pivot_index + 1));
}

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
    (void)key; //Just to silence the warning. //TODO: REMOVE!
}

static void freeTournamentData(MapDataElement tournament)
{
    freeTournament((Tournament)tournament);
}

static int compareIntegerKeys(MapKeyElement key1, MapKeyElement key2) 
{
    return (*(int *) key1 - *(int *) key2);
}


static void freeGameData(MapDataElement game)
{
    freeGame((Game)game);
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
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    ChessResult error;

    Tournament tournament = createTournament(tournament_id, tournament_location,
                            max_games_per_player, &error);

    if(tournament == NULL)
    {
        if(error == CHESS_OUT_OF_MEMORY)
        {
            chessDestroy(chess);
        }
        return error;
    }

    MapResult result = mapPut(chess->tournaments, &tournament_id, tournament);
    
    if(result == MAP_ITEM_ALREADY_EXISTS)
    {   
        freeTournament(tournament);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if(result == MAP_OUT_OF_MEMORY)
    {
        freeTournament(tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(alreadyExistsInSystem(chess, first_player, second_player))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }

    Tournament tournament = mapGet(chess->tournaments, &tournament_id);
    if(tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    ChessResult error =
            addGameToTournament(tournament, first_player, second_player, winner, play_time, chess->players);
    if (error == CHESS_OUT_OF_MEMORY)
    {
        chessDestroy(chess);
    }
    return error;
}



ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{   
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament current_tournament = mapGet(chess->tournaments, &tournament_id);
    if(current_tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    
    freeTournament(current_tournament);
    MapResult result = mapRemove(chess->tournaments, &tournament_id);
    (void)result;//Just to silence the warning. //TODO: REMOVE!
    
    

    //TODO: need to update the game statistics!
	
    return CHESS_SUCCESS;
}


ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
	if(player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    Player player = mapGet(chess->players, &player_id);
    if(player == NULL)
    {
        return CHESS_PLAYER_NOT_EXIST;
    }

    Tournament tournament;
    Game game;

    MAP_FOREACH(int*, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, current_tournament);
       
        MAP_FOREACH(int*, current_game, getGames(tournament) )
        {
            game = mapGet(getGames(tournament), current_game);

            if(getPlayer1Id(game) == player_id)
            {
               setPlayerForfeited(game, FIRST_PLAYER);
            }
            if(getPlayer2Id(game) == player_id)
            {
                setPlayerForfeited(game, SECOND_PLAYER);
            }

            free(current_game);
        }
        free(current_tournament);
    }

    freePlayer(player);

    MapResult result = mapRemove(chess->players, &player_id);
    (void)result;//Just to silence the warning. //TODO: REMOVE!
    
    return CHESS_SUCCESS;
}


ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament tournament = mapGet(chess->tournaments, &tournament_id);
    
    if(isClosed(tournament))
    {
        return  CHESS_TOURNAMENT_ENDED;
    }
    if(mapGetSize(getGames(tournament)) == 0)
    {
        return CHESS_NO_GAMES;   
    }

    int tournament_winner = calculateTournamentWinner(tournament);
    if(tournament_winner == NO_WINNER)
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
    double total_time = 0;
    int num_of_games =0; 

    if(player_id < 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }

    Tournament tournament;
    Game game;

    MAP_FOREACH(int*, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, current_tournament);
       
        MAP_FOREACH(int*, current_game, getGames(tournament))
        {
            game = mapGet(getGames(tournament), current_game);

            if(getPlayer1Id(game) == player_id || getPlayer2Id(game) == player_id)
            {
                total_time += getTime(game);
                num_of_games++;
            }
            free(current_game);
        }
        free(current_tournament);
    }
     
    if(num_of_games == 0)
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
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
    int size = mapGetSize(chess->players);

    double* player_levels = malloc(sizeof(*player_levels) * size);
    if(player_levels == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    int* ids = malloc(sizeof(*ids) * size);
    if(ids == NULL)
    {
        free(player_levels);
        return CHESS_OUT_OF_MEMORY;
    }

	MAP_FOREACH(int*, current_player_id, chess->players)
    {
        current_player = mapGet(chess->players, current_player_id);
        wins = getWins(current_player);
        losses = getLosses(current_player);
        draws = getDraws(current_player);

        player_level = ((double)(6*wins - 10*losses + 2*draws) / (wins + losses + draws));

        player_levels[counter] = player_level;
        ids[counter] = *current_player_id;

        free(current_player_id);
    }

    quicksort(ids, player_levels, size);

    //TODO: ensure I used fprintf correctly. Been a while...
    for(int current = 0; current < size; ++current)
    {
        fprintf(file, "%d %f\n", ids[current], player_levels[current]);
    }

    free(ids);
    free(player_levels);

    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
	return CHESS_SUCCESS;
}


bool alreadyExistsInSystem(ChessSystem chess, int first_player, int second_player)
{
    Tournament current_tournament;
    MAP_FOREACH(int*, tournament_id, chess->tournaments)
    {
        current_tournament = mapGet(chess->tournaments, tournament_id);
        if (alreadyExistsInTournament(current_tournament, first_player, second_player))
        {
            return true;
        }

        free(tournament_id);
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

//TODO: REMOVE THIS GUY!
void warningSilencer()
{
    chessRemovePlayer(NULL, 0);
    chessEndTournament(NULL, 0);
    freeGameData(NULL);
    copyGameData(NULL);
    chessCreate();
    chessAddTournament(NULL, 0, 0, "");
    chessAddGame(NULL, 0, 0, 0, FIRST_PLAYER, 0);
    chessRemoveTournament(NULL, 0);
    chessCalculateAveragePlayTime(NULL, 0, 0);
    chessSavePlayersLevels(NULL, NULL);
    chessSaveTournamentStatistics(NULL, NULL);
}
 
