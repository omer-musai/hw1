#include "tournament.h"


struct Tournament_t
{
    unsigned int tournament_id;
    char* location;
    Map games;
    int winner;
    int max_games_per_player;
    int player_count;
    bool closed;
    
};

Tournament createTournament(int tournament_id, const char* location_str,
                            int max_games_per_player, ChessResult* error)
{
    *error = CHESS_SUCCESS;

    if(invalidLocation(location_str))
    {
        *error = CHESS_INVALID_LOCATION;
    }
    else if(max_games_per_player <= 0)
    {
        *error = CHESS_INVALID_MAX_GAMES;
    }
    else if(tournament_id <= 0)
    {
        *error = CHESS_INVALID_ID;
    }

    if (*error != CHESS_SUCCESS)
    {
        return NULL;
    }

    Tournament tournament = malloc(sizeof(*tournament));
    if (tournament == NULL)
    {
        *error = CHESS_OUT_OF_MEMORY;
        return NULL;
    }

    
    tournament->location = malloc(sizeof(*(tournament->location)) * LOCATION_MAX_LENGTH);
    if (tournament->location == NULL)
    {
        *error = CHESS_OUT_OF_MEMORY;
        free(tournament);
        return NULL;
    }

    strcpy(tournament->location, location_str);

    //not exactly sure which functions to use here
    tournament->games = mapCreate(&copyGame,
                        &copyGameId,
                        &freeGame,
                        &freeGameKey,
                        &compareGameKeys);
   
    if (tournament->games == NULL)
    {
        free(tournament->location);
        free(tournament);
        *error = CHESS_OUT_OF_MEMORY;
        return NULL;
    }

    tournament->max_games_per_player = max_games_per_player;
    tournament->player_count = 0;
    tournament->tournament_id = tournament_id;  
    tournament->closed = false;

    return tournament;        
}


void freeTournament(Tournament tournament)
{
    if(tournament == NULL)
    {
        return NULL;
    }

    free(tournament->location);
    mapDestroy(tournament->games);
    free(tournament);
    return;
}


//Getters & setters:
int getId(Tournament tournament)
{
    return tournament->tournament_id;
}
int getTournamentWinner(Tournament tournament);
{
    return tournament->winner;
}
Map getGames(Tournament tournament)
{
    return tournament->games;
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


Tournament copyTournament(Tournament src)
{
    if (src == NULL)
    {
        return NULL;
    }

    Tournament copy = malloc(sizeof(*copy));
    if (copy == NULL)
    {
        return NULL;
    }

    copy->closed = src->closed;
    copy->tournament_id = src->tournament_id;
    copy->player_count = src->player_count;
    copy->games = mapCopy(src->games);
    copy->winner = src->winner;
    copy->max_games_per_player = src->max_games_per_player;
    copy->location = malloc(sizeof(*(copy->location)) * LOCATION_MAX_LENGTH);
    if(copy->location == NULL)
    {
        return NULL;
    }

    strcpy(copy->location, src->location);

    return copy;
}

void setTournamentWinner(Tournament tournament, int winner)
{
    tournament->winner = winner;
    return;
}

void close(Tournament tournament)
{
    tournament->closed = true;
}
bool isClosed(Tournament tournament)
{
    return tournament->closed;
}


//additional functions
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

bool invalidLocation(const char* tournament_location)
{
    if(tournament_location == NULL)
    {
        return true;
    }
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

//check if there is a limit of 30 lines...
Map getTournamentPlayers(Tournament tournament)
{
    Map players_in_tournament = mapCreate(&copyPlayer,
                                            &copyPlayerId,
                                            &freePlayer,
                                            &freePlayerKey,
                                            &comparePlayerKeys);

    if(players_in_tournament == NULL)
    {
        return NULL;
    }

     Map games = getGames(tournament);
     Game game;
     Player player;
     int current_player1;
     int current_player2;
     
      
        MAP_FOREACH(int, current_game, games)
        {
            game = mapGet(games, &current_game);
            current_player1 = getPlayer1Id(game);
            current_player2 = getPlayer2Id(game);
            

            if(!mapContains(players_in_tournament, current_player1))
            {
                player = createPlayer(getPlayer1Id(game));
                MapResult result = mapPut(players_in_tournament, current_player1, player);
                if(result == MAP_OUT_OF_MEMORY)
                {
                    mapDestroy(players_in_tournament);
                    return NULL;
                }
            }
            if(!mapContains(players_in_tournament, current_player2))
            {
                player = createPlayer(getPlayer2Id(game));
                MapResult result = mapPut(players_in_tournament, current_player2, player);
                if(result == MAP_OUT_OF_MEMORY)
                {
                    mapDestroy(players_in_tournament);
                    return NULL;
                }
            }

            Player player1 = mapGet(players_in_tournament, current_player1);
            Player player2 = mapGet(players_in_tournament, current_player2);
            Winner winner = getWinner(game);
           
            if(winner == FIRST_PLAYER)
            {
                int wins = getWins(player1);
                int losses = getLosses(player2);
                
                setWins(player1, wins + 1);
                setLosses(player2, losses + 1);
            }
            else if(winner == SECOND_PLAYER)
            {     
                int wins = getWins(player2);
                int losses = getLosses(player1);
                
                setWins(player2, wins + 1);
                setLosses(player1, losses + 1);
            }
            else
            {
                int draws1 = getDraws(player1);
                int draws2 = getDraws(player2);
                setDraws(player1, draws1 + 1);
                setDraws(player2, draws2 + 1);
            }
        }
        return players_in_tournament;
}






int calculateTournamentWinner(Tournament tournament)
{
     Player current_player;
     Player current_winner;
     int current_winner_score = -1;
     int current_winner_id;

    Map players_in_tournament = getTournamentPlayers(tournament);
    if(players_in_tournament == NULL)
    {
        return NULL;
    }

     

        MAP_FOREACH(int, current_player_id, players_in_tournament)
        {
            current_player = mapGet(players_in_tournament, &current_player_id);
            current_winner = mapGet(players_in_tournament, &current_winner_id);

            if(playerScore(current_player) > current_winner_score)
            {
               current_winner_score = playerScore(current_player);
               current_winner_id = &current_player_id;
            }
            else if(playerScore(current_player) == current_winner_score)
            {
                if(getLosses(current_player) < getLosses(current_winner))
                {
                    current_winner_score = playerScore(current_player);
                    current_winner_id = &current_player_id;
                }
                else if(getLosses(current_player) == getLosses(current_winner))
                {
                    if(getwins(current_player) > getwins(current_winner))
                    {
                        current_winner_score = playerScore(current_player);
                        current_winner_id = &current_player_id;
                    }
                    else if(getwins(current_player) == getwins(current_winner))
                    {
                        if(current_player_id < current_winner_id)
                        {
                            current_winner_score = playerScore(current_player);
                            current_winner_id = &current_player_id;
                        }
                    }
                }
                
            }
        }

        mapDestroy(players_in_tournament);
        return current_winner_id;   
}

void freeTournamentKey(int key)
{
    //Our key is an integer so nothing needs to be done.
}

int copyTournamentId(int id)
{
    int copy = id;
    
    return copy;
}
int compareTournamentKeys(int key1, int key2)
{
    return (key1 - key2);
}

