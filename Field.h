#pragma once

#include "stdafx.h"

class Field
{
	int mapWidth;
	int mapHeight;
	char ** map;
	Coordinates robot;
	CoordinatesVector lambdas;
	Coordinates lift;
	bool liftIsOpen;
public:
	Field(void);
	Field(const Field & field);
	~Field(void);

	int LoadMap(char * file);		// loads map from file; fills Field's fields =)
	void saveMap(ostream &sout);

	int GetWidth();
	int GetHeight();
	char ** GetMap();				// returns pointer to map
	Coordinates GetRobot();		// returns robot coordinates
	CoordinatesVector GetLambdas();		// returns list of lambda's coordinates for all lambdas on map
	Coordinates GetLift();					// returns lift coordinates
	bool isLiftOpened();			// returns the state of the lift

	void UpdateMap();				// updates map according to the rules
	bool isWalkable(int x, int y);
};
