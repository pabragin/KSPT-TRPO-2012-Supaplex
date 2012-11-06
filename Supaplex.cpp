// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

const int iterations = 1;

void start(istream & sin);


int main(int argc, char* argv[])
{
//	ofstream fout("..//IO files//output.txt");
//	fout.close();

	ifstream fin;
	if (argc == 2) {
		fin.open(argv[1]);
		if (!fin.is_open()) {
			cout << "Can't open file." << endl;
			return -1;
		}
		start(fin);
	} else if (argc != 1) {
		cout << "Usage: supaplex [input_file]" << endl;
		return -2;
	} else {
		start(cin);
	}

	return 0;
}

void start(istream & sin) {
	Game game;
	if (game.Init(sin) != -1) {
		game.Solve(iterations);
	}
}