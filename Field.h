#pragma once

#include "stdafx.h"

class Field
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
	Field(const Field & field);
	~Field(void);

	int LoadMap(char * file);		// loads map from file; fills Field's fields =)
	void SaveMap(ostream &sout);

	void SetRobot(int x, int y);	// changes robot coordinates
	void SetLiftState(bool isOpen);
	void ClearLambdas();
	void AddLambda(pair<int, int> lambda);
	void PopBackLambda();
	int FindLambda(pair<int, int> lambda);

	int GetWidth();
	int GetHeight();
	char ** GetMap();				// returns pointer to map
	pair<int, int> GetRobot();		// returns robot coordinates
	vector<pair<int, int>> GetLambdas();		// returns list of lambda's coordinates for all lambdas on map
	pair<int, int> GetLift();					// returns lift coordinates
	bool isLiftOpened();			// returns the state of the lift

	bool isWalkable(int x, int y);


	Field operator = (const Field & field);
};
