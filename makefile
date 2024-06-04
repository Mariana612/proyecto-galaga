#*************************************************
# Makefile
#*************************************************

main.o: main.cpp
	g++ main.cpp -o main -lncurses -lSDL2 -lSDL2_mixer

