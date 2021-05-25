CC = gcc
OBJECTS = map.o chessSystem.o tournament.o game.o player.o chessSystemTestsExample.o
EXEC = chess
DEBUG_FLAG = -std=c99 -pedantic-errors -Wall -Werror -g
COMP_FLAG = -std=c99  -pedantic-errors -Wall -Werror -DNDEBUG

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@



chessSystemTestsExample.o: chessSystemTestsExample.c chessSystem.h test_utilities.h
	$(CC) $(DEBUG_FLAG) -c $*.c 

chessSystem.o: chessSystem.c chessSystem.h map.h tournament.h player.h
	$(CC) $(DEBUG_FLAG) -c $*.c -o chess.o

tournament.o: tournament.c map.h tournament.h player.h game.h chessSystem.h
	$(CC) $(DEBUG_FLAG) -c $*.c

game.o: game.c map.h game.h chessSystem.h tournament.h
	$(CC) $(DEBUG_FLAG) -c $*.c

player.o: player.c player.h map.h
	$(CC) $(DEBUG_FLAG) -c $*.c

map.o: map.c map.h
	$(CC) $(DEBUG_FLAG) -c $*.c

clean:
	rm -f $(OBJECTS) $(EXEC)



#we required to use test\chessSystemtestsexample.c in order to supply makefile a main function

#this one for compiling: > gcc -std=c99 -o chessSystem -Wall -pedantic-errors -Werror -DNDEBUG *.c ./tests/chessSystemTestsExample.c -L. –lmap










	


#prog: calc.o control.o main_prog.o 	gcc calc.o control.o main_prog.o -o prog
#calc.o: calc.c calc.h	gcc -c calc.c
#control.o: control.c calc.h control.h	gcc -c control.c

#main_prog.o: calc.h control.h	gcc -c main_prog.c



	