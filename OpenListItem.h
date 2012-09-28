#pragma once

class OpenListItem
{
	int x;
	int y;
	int Fcost;
	int Gcost;
	int Hcost;
public:
	OpenListItem(void);
	OpenListItem(int, int);
	OpenListItem(int, int, int, int);
	~OpenListItem(void);

	void SetX(int);
	void SetY(int);
	void SetGcost(int);
	void SetHcost(int);

	int GetX();
	int GetY();
	int GetFcost();
	int GetGcost();
	int GetHcost();

	void SetCosts(int, int);
	void CalculateFcost();
};

