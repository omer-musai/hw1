#include "game.h"

struct Game_t
{
    int id_player1;
    int id_player2;
    Winner winner;
    int game_time;
    bool auto_win;
};

Game createGame(int id_player1, int id_player2, Winner winner, int game_time, ChessResult* error)
{
   *error = CHESS_SUCCESS;

   if(id_player1 == id_player2 || id_player1 <= 0 || id_player2 <= 0)
   {
       *error = CHESS_INVALID_ID;
   }
   else if(game_time < 0)
   {
       *error = CHESS_INVALID_PLAY_TIME;
   }


   if(*error != CHESS_SUCCESS)
   {
       return NULL;
   }

   Game game = malloc(sizeof(*game));
   if (game == NULL)
   {
        *error = CHESS_OUT_OF_MEMORY;    
        return NULL;
   }

    game->id_player1 = id_player1;
    game->id_player2 = id_player2;
    game->winner = winner;
    game->game_time = game_time;
    bool auto_win = false;

    return game;
}
               
void freeGame(Game game)
{
    free(game);
}

int getPlayer1Id(Game game)
{
    return game->id_player1;
}
int getPlayer2Id(Game game)
{
    return game->id_player2;
}

Winner getWinner(Game game)
{
    return game->winner;
}

bool isPlayerForfeited(Game game)
{
    return game->auto_win;
}

void setPlayerForfeited(Game game, Winner player_to_remove)
{
    if(player_to_remove == FIRST_PLAYER)
    {
        game->winner = SECOND_PLAYER;
    }
    else
    {
        game->winner = FIRST_PLAYER;
    }

    game->auto_win = true;
    return;
}

int getTime(Game game)
{
    return game->game_time;
}

Game copyGame(Game src)
{
    if(!src)
    {
        return NULL;
    }

    Game copy = malloc(sizeof(*copy));
    if(copy==NULL)
    {
        return NULL;
    }

    copy->game_time = src->game_time;
    copy->id_player1 = src->id_player1;
    copy->id_player2 = src->id_player2;
    copy->winner = src->winner;
    copy->auto_win = src->auto_win;

    return copy;
}

void freeGameKey(int key)
{
    //Our key is an integer so nothing needs to be done.
}

int copyGameId(int id)
{
    int copy = id;
    
    return copy;
}
int compareGameKeys(int key1, int key2)
{
    return (key1 - key2);
}
