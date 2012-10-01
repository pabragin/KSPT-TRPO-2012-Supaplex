#pragma once

#include "stdafx.h"
#include "TSPSolver.h"

class Game
{
	Field map;
	int score;
	vector<char> trace;
public:
	Game(void);
	~Game(void);

	int Start(char * file);
	void Solve(const int & iterations);

private:
	void BuildPathByCoord(vector<pair<int, int>> * path);
};

