#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chessSystem.h"
#include "map.h"
#include "player.h"
#include "tournament.h"

/*
GOLLUM:
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

#define INVALID -1

struct chess_system_t
{
    Map players;
    Map tournaments;

    //Used in saveTournamentStatistics because the "no tournaments ended" takes precedence
    //over the "save failure" error.
    bool tournament_ended;
};

//Declaring those for use in chessSavePlayersLevels:
static void swapIntegers(int *a, int *b);
static void swapDoubles(double *a, double *b);
static int partition(int *ids, double *levels, int length);
static void quicksort(int *ids, double *levels, int length);

//Construction & destruction:
ChessSystem chessCreate()                 
{   
    Map tournaments = mapCreate(&mapTournamentCopy,
                                &mapTournamentIdCopy,
                                &mapTournamentDataFree,
                                &mapTournamentIdFree,
                                &mapTournamentKeyCompare);
    
    if(tournaments == NULL)
    {
        return NULL;
    }

    Map players = mapCreate(&mapPlayerCopy,
                                &mapPlayerIdCopy,
                                &mapPlayerDataFree,
                                &mapPlayerIdFree,
                                &mapPlayerKeyCompare);

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

    chess_system->tournaments = tournaments;
    chess_system->players = players;
    chess_system->tournament_ended = false;

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
    if(tournament_location == NULL || chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }

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
    freeTournament(tournament);

    if(result == MAP_OUT_OF_MEMORY)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    assert(result != MAP_NULL_ARGUMENT);
    
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    
    if(tournament_id <= 0 || first_player <= 0 || second_player <= 0)
    {
        return CHESS_INVALID_ID;
    }

    Tournament tournament = mapGet(chess->tournaments, &tournament_id);

    if (tournament == NULL)
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
     if(chess == NULL)
        {
            return CHESS_NULL_ARGUMENT;
        }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    Tournament tournament = mapGet(chess->tournaments, &tournament_id);
    if(tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    
    removeTournamentFromStatistics(tournament, chess->players);
    
    MapResult result = mapRemove(chess->tournaments, &tournament_id);

    assert(result == MAP_SUCCESS);
    
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
    ChessResult error = CHESS_SUCCESS;

    MAP_FOREACH(int*, current_tournament, chess->tournaments)
    {
        tournament = mapGet(chess->tournaments, current_tournament);
        error = removePlayer(tournament, player_id, chess->players);
        free(current_tournament);
        if (error != CHESS_SUCCESS)
        {
            break;
        }
    }

    if (error == CHESS_SUCCESS)
    {
        MapResult result = mapRemove(chess->players, &player_id); //This also frees it.
        if (result == MAP_OUT_OF_MEMORY)
        {
            error = CHESS_OUT_OF_MEMORY;
            chessDestroy(chess);
        }
        else if (result == MAP_NULL_ARGUMENT)
        {
            error = CHESS_NULL_ARGUMENT;
        }

        assert(result != MAP_ITEM_DOES_NOT_EXIST);
    }
    
    return error;
}

ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    Tournament tournament = mapGet(chess->tournaments, &tournament_id);

    if (tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    ChessResult error = endTournament(tournament);
    if (error == CHESS_SUCCESS)
    {
        chess->tournament_ended = true;
    }
    return error;
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(chess_result == NULL || chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return INVALID;
    }

    if(player_id < 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return INVALID;
    }

    double total_time = 0;
    int num_of_games = 0, tournament_games;
    
    MAP_FOREACH(int*, current_tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, current_tournament_id);
        assert(tournament != NULL);

        total_time += getTotalPlayerPlayTime(tournament, player_id, &tournament_games);
        num_of_games += tournament_games;

        free(current_tournament_id);
    }
     
    if(num_of_games == 0)
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return INVALID;
    }

    double avg_time = (total_time / (double)num_of_games);

    *chess_result = CHESS_SUCCESS;
    return avg_time;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    double player_level;
    Player current_player;
    int wins, losses, draws;
    int counter = 0;
    int size = mapGetSize(chess->players);

    double* player_levels = malloc(sizeof(*player_levels) * size);
    if(player_levels == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    int* ids = malloc(sizeof(*ids) * size);
    if(ids == NULL)
    {
        free(player_levels);
        chessDestroy(chess);
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
        ids[counter++] = *current_player_id;
        free(current_player_id);
    }

    quicksort(ids, player_levels, size);

    ChessResult error = CHESS_SUCCESS;
    for(int current = 0; current < size && error == CHESS_SUCCESS; ++current)
    {
        if (fprintf(file, "%d %.2f\n", ids[size - 1 - current], player_levels[size - 1 - current]) < 0)
        {
            error = CHESS_SAVE_FAILURE;
        }
    }

    free(ids);
    free(player_levels);

    return error;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if(chess == NULL || path_file == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    
    bool found_finished_tournament = false; //Will be used for asserting.
    if (!chess->tournament_ended)
    {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }

    FILE *file = fopen(path_file, "w");
    if (file == NULL)
    {
        perror("??? ");
        return CHESS_SAVE_FAILURE;
    }

    MAP_FOREACH(int*, current_tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, current_tournament_id);
        assert(tournament != NULL);

        if (isFinished(tournament))
        {
            int longest_time;
            double average_time;
            getGameTimeStatistics(tournament, &longest_time, &average_time);
            found_finished_tournament = true;

            fprintf(file, "%d\n", getTournamentWinner(tournament));
            fprintf(file, "%d\n", longest_time);
            fprintf(file, "%.2f\n", average_time);
            fprintf(file, "%s\n", getLocation(tournament));
            fprintf(file, "%d\n", getGameCount(tournament));
            fprintf(file, "%d\n", getPlayerCount(tournament));
        }

        free(current_tournament_id);
    }

    assert(found_finished_tournament);

    if (fclose(file) == EOF)
    {
        return CHESS_SAVE_FAILURE;
    }
	return CHESS_SUCCESS;
}

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
            if (levels[right] == pivot)
            {
                if (ids[0] < ids[right])
                {
                    break; //Equal level, larger ID; still need to swap.
                }
            }
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


/*TODOS:
Valgrind
Constants (and generally take a look at code conventions)
Create makefile
Run tests and Valgrind on server (including finalcheck)
Look for camelCased variables
Look for TODOs\comments in the various files
Dry stuff
Remove warning-silencing stuff
Ensure error order (I guess)

More tests never hurt :)
*/

