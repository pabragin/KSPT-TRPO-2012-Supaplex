#pragma once

#include "stdafx.h"
#include "Field.h"
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
	void Solve();

private:
	void BuildPathByCoord(CoordinatesVector * path);
};

