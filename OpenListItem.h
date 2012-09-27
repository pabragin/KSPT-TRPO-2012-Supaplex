#pragma once
#include "Cell.h"

class OpenListItem :
	public Cell
{
	int Fcost;
	int Gcost;
	int Hcost;
public:
	OpenListItem(void);
	OpenListItem(int, int);
	OpenListItem(int, int, int, int);
	~OpenListItem(void);

	void SetGcost(int);
	void SetHcost(int);

	int GetFcost();
	int GetGcost();
	int GetHcost();

	void SetCosts(int, int);
	void CalculateFcost();
};

