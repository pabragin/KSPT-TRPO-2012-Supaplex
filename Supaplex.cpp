// Supaplex.cpp : Defines the entry point for the console application.
//

#include "Game.h"

char filename[] = "..//IO files//input15.txt";

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
