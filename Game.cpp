#include "Game.h"
#include "Simulator.h"


Game::Game(void)
{
	score = 0;
}


Game::~Game(void)
{
}

int Game::Init(istream &sin)
{
	int result = mine.LoadMap(sin);
	if (result != 0)
		return -1;

	score = 0;
	trace.clear();

	return 0;
}

void Game::Solve(const int & iterations)
{
	TSPSolver solver(& this->mine);
	solver.Solve(iterations);

	Simulator sim(this->mine);
	sim.StartSimulation(solver.GetNodes());

	//ofstream fout("..//IO files//output.txt", ios::app);

	vector<IntPair> path = sim.GetPath();
	BuildPathByCoord(&path);

	for (size_t i = 0; i < trace.size(); i++) {
		cout << trace[i];
	}
	cout << endl;

	//fout.close();
}

void Game::MoveRobot(_Command COMMAND)
{
	int xold = mine.GetRobot().first;
	int yold = mine.GetRobot().second;
	int x = xold, y = yold;

	switch (COMMAND) {
	case RIGHT:
		y++;
		break;
	case LEFT:
		y--;
		break;
	case UP:
		x--;
		break;
	case DOWN:
		x++;
		break;
	default:
		return;
	}

	if (x < 0 || x > mine.GetHeight() - 1 || y < 0 || y > mine.GetWidth() - 1) {
		return;
	}

	if (mine.isWalkable((int) x, (int) y)) {
		// If there is a stone in this cage
		if (mine.GetObject(x, y) == STONE) {
			if (COMMAND == RIGHT) {				// then, if robot is to the left of the stone
				mine.SetObject(x, y + 1, STONE);
			} else if (COMMAND == LEFT) {			// otherwise, if robot is to the right of the stone
				mine.SetObject(x, y - 1, STONE);
			}
		}

		mine.SetObject(xold, yold, EMPTY);
		mine.SetObject(x, y, ROBOT);
		mine.SetRobot(x, y);
	}
}

// Returns trace for the robot, like 'RRRLLLLWLLA'
void Game::BuildPathByCoord(vector<IntPair> * path)
{
	int x = mine.GetRobot().first;
	int y = mine.GetRobot().second;
	for (int i = 0; i < (int) path->size(); i++) {
		if (x - path->at(i).first == 1)
			trace.push_back(UP);
		else if (x - path->at(i).first == -1)
			trace.push_back(DOWN);
		else if (y - path->at(i).second == 1)
			trace.push_back(LEFT);
		else if (y - path->at(i).second == -1)
			trace.push_back(RIGHT);
		else if (i != (int) path->size() - 1)
			trace.push_back(WAIT);

		x = path->at(i).first;
		y = path->at(i).second;
	}

	if (x != mine.GetLift().first || y != mine.GetLift().second) {
		trace.push_back(ABORT);
	}
}
