// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

//char * filename = "..//IO Files//input10x14.txt";
char * filename = "Maps//contest8.mine";
const int & iterations = 10;

int main(int argc, char* argv[])
{
	Game game;
	if (game.Start(filename) != -1) {
		game.Solve(iterations);
	}

	return 0;
}