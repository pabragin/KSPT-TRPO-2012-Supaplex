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
	int tourSize = solver.GetTour()->size();

	BuildPathByCoord(solver.GetTourPath());

	for (int i = 0; i < trace.size(); i++) {
		cout << trace[i] << " ";
	}
	cout << endl;
}

// Returns trace for the robot, like 'RRRLLLLWLLA'
void Game::BuildPathByCoord(vector<pair<int, int>> * path)
{
	int x = map.GetRobot().first;
	int y = map.GetRobot().second;
	for (int i = 0; i < (int) path->size(); i++) {
		if (x - path->at(i).first == 1)
			trace.push_back('U');
		else if (x - path->at(i).first == -1)
			trace.push_back('D');
		else if (y - path->at(i).second == 1)
			trace.push_back('L');
		else if (y - path->at(i).second == -1)
			trace.push_back('R');
		else if (i != (int) path->size() - 1)
			trace.push_back('W');

		x = path->at(i).first;
		y = path->at(i).second;
	}

	if (x != map.GetLift().first || y != map.GetLift().second) {
		trace.push_back('A');
	}
}
