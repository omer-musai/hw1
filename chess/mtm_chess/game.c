#include "game.h"

struct Game_t
{
    int id_player1;
    int id_player2;
    Winner winner;
    double game_time;
    bool auto_win;
};

Game createGame(int id_player1, int id_player2, Winner winner, double game_time)
{
   Game game = malloc(sizeof(*game));
   if (game == NULL)
   {
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

void setPlayerForfeited(Game game, int player_to_remove)
{
    game->auto_win = true;
    return;
}

double getTime(Game game)
{
    return game->game_time;
}



