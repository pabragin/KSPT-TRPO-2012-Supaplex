// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

//char * filename = "..//IO Files//input10x14.txt";
char * filename = "Maps//contest10.mine";
const int & iterations = 1;

int main(int argc, char* argv[])
{
	Game game;
	game.Start(filename);
	game.Solve(iterations);

	return 0;
}