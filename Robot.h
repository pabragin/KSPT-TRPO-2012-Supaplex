#pragma once

#include "stdafx.h"

class Robot
{
	Cell * coord;
	list<Cell> * lambdas;
public:
	Robot(void);
	~Robot(void);

	void init(Cell * robotCoord, list<Cell> * lambdasList);
	void Move(char ** map);
};

