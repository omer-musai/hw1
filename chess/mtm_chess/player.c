#include "player.h"

struct Player_t
{
    int player_id;
    int wins;
    int losses;
    int draws;

};

//Construction & destruction:
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



//Getters & Setters:
int getWins(Player player)
{
    assert(player != NULL);
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->wins;
}

int getLosses(Player player)
{
    assert(player != NULL);
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->losses;
}

int getDraws(Player player)
{
    assert(player != NULL);
    if (player == NULL)
    {
        return ILLEGAL_PLAYER;
    }

    return player->draws;
}

void setWins(Player player, int wins)
{
    assert(player != NULL);
    if (player != NULL && wins >=0)
    {
        player->wins = wins;
    }
}

void setLosses(Player player, int losses)
{
    assert(player != NULL);
    if (player != NULL && losses >=0)
    {
        player->losses = losses;
    }
}

void setDraws(Player player, int draws)
{
    assert(player != NULL);
    if (player != NULL && draws >= 0)
    {
        player->draws = draws;
    }
}

//additional functions:
void increaseWins(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        ++(player->wins);
    }
}

void increaseDraws(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        ++(player->draws);
    }
}

void increaseLosses(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        ++(player->losses);
    }
}

void decreaseWins(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        --(player->wins);
    }
}

void decreaseDraws(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        --(player->draws);
    }
}

void decreaseLosses(Player player)
{
    assert(player != NULL);
    if (player != NULL)
    {
        --(player->losses);
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

//Map-related functions:
MapDataElement mapPlayerCopy(MapDataElement player)
{
    return copyPlayer((Player)player);
}

MapKeyElement mapPlayerIdCopy(MapKeyElement id)
{
    int *new = malloc(sizeof(int));
    if (new == NULL)
    {
        return NULL;
    }
    *new = *((int*)id);
    return new;
}

void mapPlayerDataFree(MapDataElement player)
{
    freePlayer((Player)player);
}

void mapPlayerIdFree(MapKeyElement id)
{
    free((int*)id);
}

int mapPlayerKeyCompare(MapKeyElement id1, MapKeyElement id2)
{
    return (*((int*)id1) - *((int*)id2));
}

