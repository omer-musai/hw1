#include "player.h"

typedef struct Player_t
{
    int player_id;
    int wins;
    int losses;
    int draws;

};

Player createPlayer(int player_id)
{
    Player player = malloc(sizeof(*player));
    if (player == NULL)
    {
        return NULL;
    }

    player->player_id = player_id;
    player->wins = 0;
    player->losses = 0;
    player->draws = 0;

    return player;
}

void freePlayer(Player player)
{
    free(player); //As promised :)
}

int getWins(Player player)
{
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->wins;
}

void setWins(Player player, int wins)
{
    if (player != NULL && wins >=0)
    {
        player->wins = wins;
    }
}

void increaseWins(Player player)
{
    if (player != NULL)
    {
        ++(player->wins);
    }
}

int getDraws(Player player)
{
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->draws;
}

void setDraws(Player player, int draws)
{
    if (player != NULL && draws >= 0)
    {
        player->draws = draws;
    }
}

void increaseDraws(Player player)
{
    if (player != NULL)
    {
        ++(player->draws);
    }
}

int getLosses(Player player)
{
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->losses;
}

void setLosses(Player player, int losses)
{
    if (player != NULL && losses >=0)
    {
        player->losses = losses;
    }
}

void increaseLosses(Player player)
{
    if (player != NULL)
    {
        ++(player->losses);
    }
}

Player copyPlayer(Player player)
{
    if(!player)
    {
        return NULL;
    }

    Player copy = malloc(sizeof(*copy));
    if(copy==NULL)
    {
        return NULL;
    }
    copy->player_id = player->player_id;
    copy->wins = player->wins;
    copy->losses = player->losses;
    copy->draws = player->draws;

    return copy;
}

int playerScore(Player player)
{
    return ((2 * player->wins) + player->draws);
}
void freePlayerKey(int key)
{
    //Our key is an integer so nothing needs to be done.
}

int copyPlayerId(int id)
{
    int copy = id;
    
    return copy;
}
int comparePlayerKeys(int key1, int key2)
{
    return (key1 - key2);
}


