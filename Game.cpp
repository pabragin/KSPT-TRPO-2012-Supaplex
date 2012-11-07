#include "Game.h"
#include "Simulator.h"


Game::Game(void)
{
	score = 0;
	moves = 0;
	lambdas_collected = 0;
	game_result = 0;
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
	moves = 0;
	lambdas_collected = 0;
	trace.clear();

	return 0;
}

Field Game::GetField(void)
{
	return this->mine;
}

int Game::GetScore(void)
{
	return this->score;
}
int Game::GetMoves(void)
{
	return this->moves;
}
vector<_Command> Game::GetTrace(void)
{
	return this->trace;
}

_GameResult Game::GetResult()
{
	return this->game_result;
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
	case ABORT:
		UpdateScore(false, true);
		game_result = ABORT_ESCAPE;
	default:
		moves++;
		return;
	}

	if (x < 0 || x > mine.GetHeight() - 1 || y < 0 || y > mine.GetWidth() - 1) {
		return;
	}

	if (mine.isWalkable((int) x, (int) y)) {
		// If there is a stone in this cage
		if (mine.GetObject(x, y) == STONE) {
			PushStone(COMMAND);
		} else if (mine.GetObject(x, y) == LAMBDA) {
			lambdas_collected++;
			mine.EraseLambda(IntPair(x, y));
			UpdateScore(true);
		} else if (mine.GetObject(x, y) == OPENED_LIFT) {
			UpdateScore(false, false, true);
			game_result = LIFT_ESCAPE;
		}

		mine.SetObject(xold, yold, EMPTY);
		mine.SetObject(x, y, ROBOT);
		mine.SetRobot(x, y);
	}
	mine.UpdateMap();
	
	if (mine.IsRobotDead()) game_result = DEATH_ESCAPE;
	moves++;
}

void Game::PushStone(_Command DIRECTION)
{
	int x = mine.GetRobot().first;
	int y = mine.GetRobot().second;
	if (DIRECTION == RIGHT) {				// then, if robot is to the left of the stone
		mine.SetObject(x, y + 2, STONE);
	} else if (DIRECTION == LEFT) {			// otherwise, if robot is to the right of the stone
		mine.SetObject(x, y - 2, STONE);
	}
}


void Game::UpdateScore(bool lambda_collected, bool escape_by_abort, bool escape_by_lift)
{
	if (lambda_collected) {
		score += LAMBDA_COST;
	} else if (escape_by_abort) {
		score += lambdas_collected * ABORT_COST;
		return;
	} else if (escape_by_lift) {
		score += lambdas_collected * LIFT_COST;
	}
	score += MOVE_COST;
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
