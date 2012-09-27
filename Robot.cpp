#include "Robot.h"


Robot::Robot(void)
{
	coord = NULL;
	lambdas = NULL;
}


Robot::~Robot(void)
{
}

void Robot::init(Cell * robotCoord, list<Cell> * lambdasList)
{
	this->coord = robotCoord;
	this->lambdas = lambdasList;
}

void Robot::Move(char ** map)
{
}
