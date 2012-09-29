#pragma once

#include "stdafx.h"

class Field																								// TBD: loading non-rectangular maps
{
	int mapWidth;
	int mapHeight;
	char ** map;
	pair<int, int> robot;
	vector<pair<int, int>> lambdas;
	pair<int, int> lift;
	bool liftIsOpen;
public:
	Field(void);
	~Field(void);

	int LoadMap(char * file);		// loads map from file; fills Field's fields =)
	int GetWidth();
	int GetHeight();
	char ** GetMap();				// returns pointer to map
	pair<int, int> GetRobot();				// returns robot coordinates
	vector<pair<int, int>> GetLambdas();		// returns list of lambda's coordinates for all lambdas on map
	pair<int, int> GetLift();					// returns lift coordinates
	bool isLiftOpened();			// returns the state of the lift

	void UpdateMap();				// updates map according to the rules
	bool isWalkable(int x, int y);

	void saveMap(ostream &sout);
};
