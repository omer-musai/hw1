#include <stdio.h>
#include <stdlib.h>

#include "chessSystem.h"

int main() {
    ChessSystem chess = chessCreate();
    if (chess == NULL)
    {
        printf("Null :(\n");
    }
    else
    {
        printf("Hot damn!\n");
        chessAddTournament(chess, 1, 3, "Tokyo");
        chessAddGame(chess, 1, 13, 31, SECOND_PLAYER, 15);
        chessAddGame(chess, 1, 31, 11, SECOND_PLAYER, 85);
        chessEndTournament(chess, 1);
        chessSaveTournamentStatistics(chess, "C:\\Users\\ShayH\\OneDrive\\Desktop\\hw1\\test.txt");
        chessDestroy(chess);
    }
    return 0;
}
