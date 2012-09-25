#pragma once

class Cell
{
protected:
	int x;
	int y;
public:
	Cell(void);
	Cell(int, int);
	~Cell(void);

	void SetX(int);
	void SetY(int);
	int GetX();
	int GetY();
};

