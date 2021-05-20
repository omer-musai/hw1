#include "tournament.h"


struct Tournament_t
{
    unsigned int tournament_id;
    char* location;
    Map games;
    int winner;
    int max_games_per_player;
    int player_count;
};

Tournament createTournament(int tournament_id,char* location_str,
                            int max_games_per_player, TournamentErrors *error)
{
    *error = NONE;

    if(invalidLocation(location_str))
    {
        *error = INVALID_LOCATION;
    }
    else if(max_games_per_player <= 0)
    {
        *error = INVALID_MAX_GAMES;
    }
    else if(tournament_id <= 0)
    {
        *error = INVALID_ID;
    }

    if (*error != NONE)
    {
        return NULL;
    }

    Tournament tournament = malloc(sizeof(*tournament));
    if (tournament == NULL)
    {
        *error = MEMORY_ALLOCATION;
        return NULL;
    }

    
    tournament->location = malloc(sizeof(*(tournament->location)) * LOCATION_MAX_LENGTH);
    if (tournament->location == NULL)
    {
        *error = MEMORY_ALLOCATION;
        free(tournament);
        return NULL;
    }

    strcpy(tournament->location, location_str);

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

    return tournament;        
}

void freeTournament(Tournament tournament)
{
    free(tournament);
    return;
}

/*
void destroyTournament(Tournament tournament)
{
    free(tournament->location);
    mapDestroy(tournament->games);
    free(tournament);
    return;
}
*/

//Getters & setters:
int getId(Tournament tournament)
{
    return tournament->tournament_id;
}
int calculateWinner(Tournament tournament)
{
    return tournament->winner;
}

//Remember to free the return value of the following getters(relevant for the user):
char* getLocation(Tournament tournament)
{
    return tournament->location;
}
int getPlayerGameLimit(Tournament tournament)
{
    return tournament->max_games_per_player;
}
int getPlayerCount(Tournament tournament)
{
    return tournament->player_count;
}


bool alreadyExistsInTournament(Map games, int first_player,int second_player)
{
    Game game;

    MAP_FOREACH(int, current_key, games)
    {
        game = mapGet(games, &current_key);
        
        if(((getPlayer1Id(game) == first_player && getPlayer2Id(game) == second_player) ||
            (getPlayer2Id(game) == first_player && getPlayer1Id(game) == second_player))  && !isPlayerForfeited(game))
            {
                return true;
            }
    }
    return false;
}

bool playedMaximumGames(Tournament tournament, int player)
{   
    Game game;
    int gamesPlayed = 0;

    MAP_FOREACH(int, current_key, tournament->games)
    {
        game = mapGet(tournament->games, &current_key);
        if(getPlayer1Id(game) == player || getPlayer2Id(game) == player)
        {
            gamesPlayed++;
        }
    }
    return (gamesPlayed >= tournament->max_games_per_player);
}

bool invalidLocation(char* tournament_location)
{
    int i = 1;
    if(tournament_location[0] <= 'Z' && tournament_location[0] >= 'A')
    {
        while(tournament_location[i] != NULL)
        {
            if((tournament_location[i] <= 'z' && tournament_location[i] >= 'a') || tournament_location[i] == ' ')
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

