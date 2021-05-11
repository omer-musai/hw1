#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "chessSystem.h"
#include "map.h"

struct chess_system_t
{
    Map tournaments;
              
};

typedef struct game_t
{
    unsigned int game_id;
    int id_player1;
    int id_player2;
    Winner score;
    int game_time;

} Game;

typedef struct tournament_t
{
    unsigned int tournament_id;
    char* location;
    Map games;
    int winner;

} Tournament;

//function declaration
bool invalidLocation(char* tournament_location);

ChessSystem chessCreate()
{    
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
    free(chess);
    return;
	
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    MapResult result = mapPut(chess, tournament_id, tournament_location);
    
    if(result == MAP_ITEM_DOES_NOT_EXIST)
    {
        return CHESS_INVALID_ID; 
    }
    else if(result == MAP_ITEM_ALREADY_EXISTS)
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    else if(invalidLocation(tournament_location))
    {
        return CHESS_INVALID_LOCATION;
    }
    else if(max_games_per_player <= 0)
    {
        return CHESS_INVALID_MAX_GAMES;
    }
    else
    {
        return CHESS_SUCCESS;
    }
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    
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
