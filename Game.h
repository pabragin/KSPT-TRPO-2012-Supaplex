#pragma once

#include "stdafx.h"
#include "TSPSolver.h"

class Game
{
	Field mine;

	int score;
	int moves;
	int lambdas_collected;

	vector<_Command> trace;

	_GameResult game_result;
public:
	Game(void);
	~Game(void);

	Field *GetField();
	int GetScore();
	int GetMoves();
	int GetCollectedLambdasNum();

	vector<_Command> GetTrace();
	_GameResult GetResult();
//	void SetTrace(vector<_Command> trac);

	int Init(istream &sin);
	void Solve(const int & iterations);

	void MoveRobot(_Command COMMAND);

private:
	void PushStone(_Command DIRECTION);
	void UpdateScore(bool lambda_collected = false, bool escape_by_abort = false, bool escape_by_lift = false);
	void BuildPathByCoord(vector<IntPair> * path);
};

