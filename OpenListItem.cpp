#include "OpenListItem.h"


OpenListItem::OpenListItem(void)
{
	x = y = -1;
	Fcost = Gcost = Hcost = 0;
}
OpenListItem::OpenListItem(int xCoord, int yCoord)
{
	x = xCoord;
	y = yCoord;
}
OpenListItem::OpenListItem(int xCoord, int yCoord, int aGcost, int aHcost)
{
	x = xCoord;
	y = yCoord;
	Gcost = aGcost;
	Hcost = aHcost;
	Fcost = Gcost + Hcost;
}

OpenListItem::~OpenListItem(void)
{
}

void OpenListItem::SetGcost(int aGcost)
{
	this->Gcost = aGcost;
}

void OpenListItem::SetHcost(int aHcost)
{
	this->Hcost = aHcost;
}

int OpenListItem::GetFcost()
{
	return Fcost;
}

int OpenListItem::GetGcost()
{
	return Gcost;
}

int OpenListItem::GetHcost()
{
	return Hcost;
}

void OpenListItem::SetCosts(int aGcost, int aHcost)
{
	this->Gcost = aGcost;
	this->Hcost = aHcost;
	CalculateFcost();
}

void OpenListItem::CalculateFcost()
{
	this->Fcost = Gcost + Hcost;
}
