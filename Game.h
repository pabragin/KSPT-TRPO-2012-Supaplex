#pragma once

#include "stdafx.h"
#include "TSPSolver.h"

class Game
{
	Field mine;
	int score;
	int moves;
	//int lambdas;
	vector<_Command> trace;
public:
	Game(void);
	~Game(void);
	Field GetField();
	int GetScore();
	int GetMoves();
	vector<_Command> GetTrace();
	void SetTrace(vector<_Command> trac);
	int Init(istream &sin);
	void Solve(const int & iterations);

	void MoveRobot(_Command DIRECTION);

private:
	void BuildPathByCoord(vector<IntPair> * path);
};

