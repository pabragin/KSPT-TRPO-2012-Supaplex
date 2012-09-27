#pragma once

#include "stdafx.h"

class Field
{
	int mapWidth;
	int mapHeight;
	char ** map;
	Cell robot;
	list<Cell> lambdas;
	Cell lift;
	bool liftIsOpen;
public:
	Field(void);
	~Field(void);

	int LoadMap(char * file);		// loads map from file; fills Field's fields =)
	int GetWidth();
	int GetHeight();
	char ** GetMap();				// returns pointer to map
	Cell GetRobot();				// returns robot coordinates
	list<Cell> GetLambdas();		// returns list of lambda's coordinates for all lambdas on map
	Cell GetLift();					// returns lift coordinates
	bool isLiftOpened();			// returns the state of the lift

	void UpdateMap();				// updates map according to the rules
	bool isWalkable(int x, int y);

	void printMap(ostream &sout);
};
