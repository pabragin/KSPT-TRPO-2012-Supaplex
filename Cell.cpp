#include "Cell.h"


Cell::Cell(void)
{
	x = y = -1;
}
Cell::Cell(int xCoord, int yCoord)
{
	x = xCoord;
	y = yCoord;
}

Cell::~Cell(void)
{
}

void Cell::SetX(int X)
{
	if (X >= 0) this->x = X;
}

void Cell::SetY(int Y)
{
	if (Y >= 0) this->y = Y;
}

int Cell::GetX()
{
	return x;
}

int Cell::GetY()
{
	return y;
}
