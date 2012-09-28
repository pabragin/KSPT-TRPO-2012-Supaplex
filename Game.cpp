#include "Game.h"


Game::Game(void)
{
	score = 0;
}


Game::~Game(void)
{
}

int Game::Start(char * file)
{
	int result = map.LoadMap(file);
	if (result != 0) {
		cout << "Unknown file." << endl;
		return -1;
	}

//	map.printMap(cout);

	return 0;
}

void Game::Solve()
{
	TSPSolver solver(this->map);
	solver.Solve();
	int tourSize = solver.GetTour().size();

	for (int i = 0; i < tourSize; i++) {
		cout << solver.GetTour()[i] << " ";
	}
	cout << endl << endl;

	vector<pair<int, int>> tour = solver.GetNodes();
	for (int i = 0; i < tourSize; i++) {
		cout << tour[solver.GetTour()[i]].first << ":" << tour[solver.GetTour()[i]].second << endl;
	}
	cout << endl;


	//if (!map.GetMap()) return;
	//while (true) {
	//	robot.Move(map.GetMap());
	//	map.UpdateMap();
	//}
}
