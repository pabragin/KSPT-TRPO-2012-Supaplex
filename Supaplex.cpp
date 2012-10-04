// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

char ** filename;
const int filesNumber = 10;

const int iterations = 10;

int main(int argc, char* argv[])
{
	ofstream fout("..//IO files//output.txt");
	fout.close();

	Game game;

	filename = new char* [filesNumber];
	filename[0] = "Maps//contest1.mine";
	filename[1] = "Maps//contest2.mine";
	filename[2] = "Maps//contest3.mine";
	filename[3] = "Maps//contest4.mine";
	filename[4] = "Maps//contest5.mine";
	filename[5] = "Maps//contest6.mine";
	filename[6] = "Maps//contest7.mine";
	filename[7] = "Maps//contest8.mine";
	filename[8] = "Maps//contest9.mine";
	filename[9] = "Maps//contest10.mine";

	for (int i = 0; i < filesNumber; i++) {
		if (game.Start(filename[i]) != -1) {
			game.Solve(iterations);
		}
	}

	return 0;
}