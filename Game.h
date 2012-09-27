#pragma once

#include "stdafx.h"
#include "Field.h"
#include "Robot.h"

class Game
{
	Field map;
	Robot robot;
	int score;
	list<char> path;
public:
	Game(void);
	~Game(void);

	int Start(char * file);
	void Solve();
};

