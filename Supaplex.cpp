// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

char filename[] = "..//IO files//input15.txt";

int main(int argc, char* argv[])
{
	Game game;
	game.Start(filename);
	game.Solve();

	return 0;
}
