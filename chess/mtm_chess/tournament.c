#include "tournament.h"

//Checks if player had already played the maximum amount of games allowed in the given tournament.
static bool playedMaximumGames(Tournament tournament, int player);
//Checks location validity.
static bool invalidLocation(const char* tournament_location);
static bool isPlayersFirstGame(Tournament tournament, int player_id);
//Updates the statistics (wins/losses/draws) of a given player based on a given game.
//Only meant to be used when adding a game to a tournament, NOT ON PLAYER REMOVAL.
static ChessResult updatePlayerStatistics(Game game, Map players, int player_id);
//Calls the previous function on both players of a given game.
static ChessResult updatePlayersStatistics(Game game, Map players);
static void setTournamentWinner(Tournament tournament, int winner);

struct Tournament_t
{
    unsigned int tournament_id;
    char* location;
    Map games;
    int winner;
    int max_games_per_player;
    int player_count;
    bool finished;
    
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

    
    tournament->location = malloc(sizeof(*(tournament->location)) * strlen(location_str));
    if (tournament->location == NULL)
    {
        *error = CHESS_OUT_OF_MEMORY;
        free(tournament);
        return NULL;
    }

    strcpy(tournament->location, location_str);

    tournament->games = mapCreate(&mapGameCopy,
                        &mapGameIdCopy,
                        &mapGameDataFree,
                        &mapGameIdFree,
                        &mapGameKeyCompare);
   
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
    tournament->finished = false;

    return tournament;        
}


void freeTournament(Tournament tournament)
{
    if(tournament == NULL)
    {
        return;
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

int getTournamentWinner(Tournament tournament)
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

    copy->finished = src->finished;
    copy->tournament_id = src->tournament_id;
    copy->player_count = src->player_count;
    copy->games = mapCopy(src->games);
    copy->winner = src->winner;
    copy->max_games_per_player = src->max_games_per_player;
    copy->location = malloc(sizeof(*(copy->location)) * strlen(src->location));
    if(copy->location == NULL)
    {
        return NULL;
    }

    strcpy(copy->location, src->location);

    return copy;
}

static void setTournamentWinner(Tournament tournament, int winner)
{
    tournament->winner = winner;
    return;
}

ChessResult endTournament(Tournament tournament)
{
    if(isFinished(tournament))
    {
        return  CHESS_TOURNAMENT_ENDED;
    }
    if(getGameCount(tournament) == 0)
    {
        return CHESS_NO_GAMES;
    }

    int winner = calculateTournamentWinner(tournament);
    assert(winner != NO_WINNER);

    tournament->finished = true;

    return CHESS_SUCCESS;
}

bool isFinished(Tournament tournament)
{
    return tournament->finished;
}

static bool isPlayersFirstGame(Tournament tournament, int player_id)
{
    bool found = false;
    MAP_FOREACH(int*, current_game_id, tournament->games)
    {
        Game current_game = mapGet(tournament->games, current_game_id);
        free(current_game_id);

        if ((found = didPlayerPlay(current_game, player_id)))
        {
            break;
        }
    }
    return found;
}

static ChessResult updatePlayersStatistics(Game game, Map players)
{
    ChessResult error;

    error = updatePlayerStatistics(game, players, getPlayer1Id(game));
    if (error != CHESS_SUCCESS)
    {
        return error;
    }

    return updatePlayerStatistics(game, players, getPlayer2Id(game));
}

static ChessResult updatePlayerStatistics(Game game, Map players, int player_id)
{
    assert(!isPlayerForfeited(game));

    if (!mapContains(players, &player_id))
    {
        Player player = createPlayer(player_id);
        if (player == NULL)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        MapResult error = mapPut(players, &player_id, player);
        if (error != MAP_SUCCESS)
        {
            free(player);
            if (error == MAP_OUT_OF_MEMORY)
            {
                return CHESS_OUT_OF_MEMORY;
            }
            assert(false /* We shouldn't ever get here. */);
        }
    }

    Player player = mapGet(players, &player_id);
    if (player == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }

    Winner winner = getWinner(game);
    if (winner == DRAW)
    {
        increaseDraws(player);
    }
    else if (getWinnerId(game) == player_id)
    {
        increaseWins(player);
    }
    else
    {
        increaseLosses(player);
    }

    return CHESS_SUCCESS;
}

ChessResult addGameToTournament(Tournament tournament, int first_player, int second_player, Winner winner, int play_time, Map players_map)
{
    ChessResult error = CHESS_SUCCESS;
    if(tournament == NULL)
    {
        error = CHESS_TOURNAMENT_NOT_EXIST;
    }
    else if(playedMaximumGames(tournament, first_player)
        || playedMaximumGames(tournament, second_player))
    {
        error = CHESS_EXCEEDED_GAMES;
    }
    else if(isFinished(tournament))
    {
        error = CHESS_TOURNAMENT_ENDED;
    }

    if (error != CHESS_SUCCESS)
    {
         return error;
    }

    Game game = createGame(first_player, second_player, winner, play_time, &error);
    if(game == NULL)
    {
        return error;
    }

    int newKey = mapGetSize(tournament->games) + 1;
    if(mapPut(tournament->games, &newKey, game) == MAP_OUT_OF_MEMORY)
    {
        freeGame(game);
        return CHESS_OUT_OF_MEMORY;
    }

    tournament->player_count +=
            isPlayersFirstGame(tournament, first_player)
            + isPlayersFirstGame(tournament, second_player);

    return updatePlayersStatistics(game, players_map);
}

ChessResult removePlayer(Tournament tournament, int player_id)
{
    MAP_FOREACH(int*, current_game_id, tournament->games)
    {
        Game game = mapGet(tournament->games, current_game_id);
        assert(game != NULL);

        if(didPlayerPlay(game, player_id))
        {
            setPlayerForfeited(game, player_id);
        }

        free(current_game_id);
    }

    return CHESS_SUCCESS;
}

//additional functions
bool alreadyExistsInTournament(Tournament tournament, int first_player,int second_player)
{
    Game game;

    MAP_FOREACH(int*, current_key, tournament->games)
    {
        game = mapGet(tournament->games, current_key);
        
        if(((getPlayer1Id(game) == first_player && getPlayer2Id(game) == second_player) ||
            (getPlayer2Id(game) == first_player && getPlayer1Id(game) == second_player))  && !isPlayerForfeited(game))
        {
            return true;
        }

        free(current_key);
    }
    return false;
}

static bool playedMaximumGames(Tournament tournament, int player)
{   
    Game game;
    int gamesPlayed = 0;

    MAP_FOREACH(int*, current_key, tournament->games)
    {
        game = mapGet(tournament->games, current_key);
        if(getPlayer1Id(game) == player || getPlayer2Id(game) == player)
        {
            gamesPlayed++;
        }

        free(current_key);
    }
    return (gamesPlayed >= tournament->max_games_per_player);
}

static bool invalidLocation(const char* tournament_location)
{
    if(tournament_location == NULL)
    {
        return true;
    }
    int i = 1;
    if(tournament_location[0] <= 'Z' && tournament_location[0] >= 'A')
    {
        while(tournament_location[i] != '\0')
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

int getGameCount(Tournament tournament)
{
    return mapGetSize(tournament->games);
}

int getTotalPlayerPlayTime(Tournament tournament, int id, int* game_count)
{
    int total_playtime = 0;
    if (game_count != NULL)
    {
        *game_count = 0;
    }

    MAP_FOREACH(int*, current_game_id, tournament->games)
    {
        Game game = mapGet(tournament->games, current_game_id);
        assert(game != NULL);

        total_playtime += getPlayerPlayTime(game, id);
        if (game_count != NULL)
        {
            ++(*game_count);
        }

        free(current_game_id);
    }

    return total_playtime;
}

void getGameTimeStatistics(Tournament tournament, int *longest_time, double *average_time)
{
    assert(getGameCount(tournament) > 0);

    int maximum_game_time = 0, total_time = 0;

    MAP_FOREACH(int*, current_game_id, tournament->games)
    {
        Game game = mapGet(tournament->games, current_game_id);
        int time = getTime(game);

        maximum_game_time = time > maximum_game_time ? time : maximum_game_time;
        total_time += time;

        free(current_game_id);
    }

    *longest_time = maximum_game_time;
    *average_time = (double)total_time/getGameCount(tournament);
}

//check if there is a limit of 30 lines...
Map getTournamentPlayers(Tournament tournament)
{
    Map players_in_tournament = mapCreate(&mapPlayerCopy,
                                            &mapPlayerIdCopy,
                                            &mapPlayerDataFree,
                                            &mapPlayerIdFree,
                                            &mapPlayerKeyCompare);

    if(players_in_tournament == NULL)
    {
        return NULL;
    }

     Map games = getGames(tournament);
     Game game;
     Player player;
     int current_player1;
     int current_player2;
     
      
        MAP_FOREACH(int*, current_game, games)
        {
            game = mapGet(games, &current_game);
            current_player1 = getPlayer1Id(game);
            current_player2 = getPlayer2Id(game);
            

            if(!mapContains(players_in_tournament, &current_player1))
            {
                player = createPlayer(getPlayer1Id(game));
                MapResult result = mapPut(players_in_tournament, &current_player1, player);
                if(result == MAP_OUT_OF_MEMORY)
                {
                    mapDestroy(players_in_tournament);
                    return NULL;
                }
            }
            if(!mapContains(players_in_tournament, &current_player2))
            {
                player = createPlayer(getPlayer2Id(game));
                MapResult result = mapPut(players_in_tournament, &current_player2, player);
                if(result == MAP_OUT_OF_MEMORY)
                {
                    mapDestroy(players_in_tournament);
                    return NULL;
                }
            }

            Player player1 = mapGet(players_in_tournament, &current_player1);
            Player player2 = mapGet(players_in_tournament, &current_player2);
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

            free(current_game);
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
        return NO_WINNER;
    }

        MAP_FOREACH(int*, current_player_id, players_in_tournament)
        {
            current_player = mapGet(players_in_tournament, current_player_id);
            current_winner = mapGet(players_in_tournament, &current_winner_id); //TODO: Check logic. Uninitialized.

            if(playerScore(current_player) > current_winner_score)
            {
               current_winner_score = playerScore(current_player);
               current_winner_id = *current_player_id;
            }
            else if(playerScore(current_player) == current_winner_score)
            {
                if(getLosses(current_player) < getLosses(current_winner))
                {
                    current_winner_score = playerScore(current_player);
                    current_winner_id = *current_player_id;
                }
                else if(getLosses(current_player) == getLosses(current_winner))
                {
                    if(getWins(current_player) > getWins(current_winner))
                    {
                        current_winner_score = playerScore(current_player);
                        current_winner_id = *current_player_id;
                    }
                    else if(getWins(current_player) == getWins(current_winner))
                    {
                        if(*current_player_id < current_winner_id)
                        {
                            current_winner_score = playerScore(current_player);
                            current_winner_id = *current_player_id;
                        }
                    }
                }
                
            }
            free(current_player_id);
        }
        mapDestroy(players_in_tournament);
        return current_winner_id;   
}

int compareTournamentKeys(int key1, int key2)
{
    return (key1 - key2);
}


//Map-related functions:
MapDataElement mapTournamentCopy(MapDataElement game)
{
    return copyGame((Game)game);
}

MapKeyElement mapTournamentIdCopy(MapKeyElement id)
{
    int *new = malloc(sizeof(int));
    if (new == NULL)
    {
        return NULL;
    }
    *new = *((int*)id);
    return new;
}

void mapGameDataFree(MapDataElement tournament)
{
    freeTournament((Tournament)tournament);
}

void mapTournamentIdFree(MapKeyElement id)
{
    free((int*)id);
}

int mapTournamentKeyCompare(MapKeyElement id1, MapKeyElement id2)
{
    return compareTournamentKeys(*((int*)id1), *((int*)id2));
}
