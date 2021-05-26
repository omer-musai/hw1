CC = gcc
OBJECTS = chess.o tournament.o game.o player.o chessSystemTestsExample.o
EXEC = chess 
DEBUG_FLAG = -g
DNDEBUG_FLAG = -DNDEBUG
COMP_FLAG = -std=c99  -pedantic-errors -Wall -Werror

$(EXEC): $(OBJECTS)
	$(CC) $(COMP_FLAG) $(DNDEBUG) $(OBJECTS) -o $@ -L. -lmap


chess.o: chessSystem.c chessSystem.h map.h tournament.h player.h
	$(CC) $(COMP_FLAG) $(DNDEBUG_FLAG) -c chessSystem.c -o chess.o

chessSystemTestsExample.o: tests/chessSystemTestsExample.c chessSystem.h test_utilities.h
	$(CC) $(COMP_FLAG) $(DNDEBUG_FLAG) -c tests/$*.c 

tournament.o: tournament.c map.h tournament.h player.h game.h chessSystem.h
	$(CC) $(COMP_FLAG) $(DNDEBUG_FLAG) -c $*.c

game.o: game.c map.h game.h chessSystem.h tournament.h
	$(CC) $(COMP_FLAG) $(DNDEBUG_FLAG) -c $*.c

player.o: player.c player.h map.h
	$(CC) $(COMP_FLAG) $(DNDEBUG_FLAG) -c $*.c

clean:
	rm -f $(OBJECTS) $(EXEC)



