// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

const int iterations = 1;

int main(int argc, char* argv[])
{
//	ofstream fout("..//IO files//output.txt");
//	fout.close();

	Game game;
	if (game.Init(cin) != -1) {
		game.Solve(iterations);
	}

	return 0;
}