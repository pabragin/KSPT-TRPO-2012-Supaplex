#pragma once

#include "stdafx.h"
#include "TSPSolver.h"

class Game
{
	Field mine;
	int score;
	vector<_Command> trace;
public:
	Game(void);
	~Game(void);

	int Init(istream &sin);
	void Solve(const int & iterations);

	void MoveRobot(_Command DIRECTION);

private:
	void BuildPathByCoord(vector<IntPair> * path);
};

