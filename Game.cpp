#include "Game.h"
#include "Simulator.h"


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

void Game::Solve(const int & iterations)
{
	TSPSolver solver(& this->map);
	solver.Solve(iterations);

	Simulator sim(& this->map);
	sim.StartSimulation(solver.GetNodes());


	int tourSize = solver.GetTour().size();

	ofstream fout("..//IO files//output.txt");


	for (int i = 0; i < sim.GetPath().size(); i++) {
		fout << sim.GetPath().at(i).first << ":" << sim.GetPath().at(i).second << endl;
	}
	fout << endl;


	//for (int i = 0; i < solver.GetTourPath().size(); i++) {
	//	fout << solver.GetTourPath()[i].first << ":" << solver.GetTourPath()[i].second << endl;
	//}
	//fout << endl;

	//for (int i = 0; i < solver.GetTour().size(); i++) {
	//	fout << solver.GetNodes()[solver.GetTour()[i]].first << ":" << solver.GetNodes()[solver.GetTour()[i]].second << endl;
	//}
	//fout << endl;

	//BuildPathByCoord(&solver.GetTourPath());

	//for (int i = 0; i < trace.size(); i++) {
	//	fout << trace[i];
	//}
	//fout << endl;

	fout.close();
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
