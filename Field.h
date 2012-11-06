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
	bool robotIsDead;

public:
	Field(void);
	Field(const Field & field);
	~Field(void);

	int LoadMap(istream &sin);		// loads map from file; fills Field's fields =)
	void SaveMap(ostream &sout);

	void SetRobot(size_t x, size_t y);	// changes robot coordinates
	void SetLiftState(bool isOpen);
	void ClearLambdas();
	void AddLambda(IntPair lambda);
	void PopBackLambda();
	int FindLambda(IntPair lambda);

	_MineObject GetObject(size_t x, size_t y);
	void SetObject(size_t x, size_t y, _MineObject OBJECT);

	int GetWidth();
	int GetHeight();
	char ** GetMap();				// returns pointer to map
	Coordinates GetRobot();		// returns robot coordinates
	CoordinatesVector GetLambdas();		// returns list of lambda's coordinates for all lambdas on map
	Coordinates GetLift();					// returns lift coordinates
	bool isLiftOpened();			// returns the state of the lift

	void UpdateMap();	// updates map according to the rules
	bool isWalkable(int x, int y);


	Field operator = (const Field & field);
};
