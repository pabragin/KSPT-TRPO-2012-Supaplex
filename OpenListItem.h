#pragma once
#include "Cell.h"

class OpenListItem :
	public Cell
{
	int Fcost;
	int Gcost;
	int Hcost;
	//int whichList; // used to record whether a cell is on the open list or on the closed list.
public:
	OpenListItem(void);
	OpenListItem(int, int);
	OpenListItem(int, int, int, int);
	~OpenListItem(void);

	void SetGcost(int);
	void SetHcost(int);
	//void SetWhichList(int);

	int GetFcost();
	int GetGcost();
	int GetHcost();
	//int GetWhichList();

	void SetCosts(int, int);
	void CalculateFcost();
};

