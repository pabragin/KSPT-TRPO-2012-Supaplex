#include "Field.h"

Field::Field(void)
{
	mapWidth = -1;
	mapHeight = -1;
	map = NULL;
	liftIsOpen = false;
	robotIsDead = false;
}

Field::Field(const Field & field)
{
	mapWidth = field.mapWidth;
	mapHeight = field.mapHeight;
	robot = field.robot;
	lambdas = field.lambdas;
	lift = field.lift;
	liftIsOpen = field.liftIsOpen;

	map = new _MineObject * [mapHeight];
	for (size_t i = 0; i < mapHeight; i++) {
		map[i] = new _MineObject [mapWidth];
		for (size_t j = 0; j < mapWidth; j++) {
			map[i][j] = field.map[i][j];
		}
	}
}

Field::~Field(void)
{
	//if (map)
	//	for (int i = 0; i < mapHeight; i++)
	//		delete [] map[i];
}

// Description: Loads map from file and fills Field's fields =)
int Field::LoadMap(istream &sin)
{
	lambdas.clear();

	vector<string> buf;
	string str;
	size_t width = 0;

	// Counting mine's dimension
	while (!sin.eof()) {
		getline(sin, str);
		buf.push_back(str);
		if (str.size() > width) width = str.size(); // remember the longest row
	}

	mapWidth = width;
	mapHeight = buf.size();
	sin.seekg(0, ios::beg);

	map = new char * [mapHeight];
	for (size_t i = 0; i < mapHeight; i++) {
		map[i] = new char [mapWidth];
		for (size_t j = 0; j < mapWidth; j++) {

			// All rows shorter than map width are supplemented with whitespaces
			if (j >= buf.at(i).size()) {
				map[i][j] = EMPTY;
				continue;
			}
			map[i][j] = buf.at(i)[j];

			if (map[i][j] == ROBOT) {				// remembering robot coordinates
				robot.first = i;
				robot.second = j;
			} else if (map[i][j] == LAMBDA)
				lambdas.push_back(IntPair(i, j));		// filling lambdas's list
			else if (map[i][j] == CLOSED_LIFT) {	// remembering closed lift coordinates
				lift.first = i;
				lift.second = j;
			} else if (map[i][j] == OPENED_LIFT) {	// remembering open lift coordinates
				lift.first = i;
				lift.second = j;
				liftIsOpen = true;
			}
		}
	}

	return 0;
}

// Description: Prints map using the specified stream
void Field::SaveMap(ostream &sout)
{
	sout << endl;
	for (size_t i = 0; i < mapHeight; i++) {
		for (size_t j = 0; j < mapWidth; j++) {
			sout << map[i][j];
		}
		sout << endl;
	}
	sout << endl;
}

// Description: Changes robot coordinates
void Field::SetRobot(size_t x, size_t y)
{
	if (x < mapHeight && y < mapWidth) {
		robot.first = x;
		robot.second = y;
	}
}

void Field::SetLiftState(bool isOpen)
{
	liftIsOpen = isOpen;
}

void Field::ClearLambdas()
{
	lambdas.clear();
}

void Field::AddLambda(IntPair lambda)
{
	lambdas.push_back(lambda);
}

void Field::PopBackLambda()
{
	lambdas.pop_back();
}

int Field::FindLambda(IntPair lambda)
{
	for (size_t i = 0; i < lambdas.size(); i++) {										// this is the worst method!!!
		if (lambdas.at(i) == lambda) return i;
	}
	return -1;
}

_MineObject Field::GetObject(size_t x, size_t y)
{
	if (x < mapHeight && y < mapWidth)
		return map[x][y];
	else
		return NULL;
}

void Field::SetObject(size_t x, size_t y, _MineObject OBJECT)
{
	if (x < mapHeight && y < mapWidth)
		map[x][y] = OBJECT;
}

// Description: Returns map width
int Field::GetWidth()
{
	return this->mapWidth;
}

// Description: Returns map height
int Field::GetHeight()
{
	return this->mapHeight;
}

// Description: Returns map
char ** Field::GetMap()
{
	return this->map;
}

// Description: Returns robot coordinates
IntPair Field::GetRobot()
{
	return this->robot;
}

// Description: Returns list of lambda's coordinates for all lambdas on map
vector<IntPair> Field::GetLambdas()
{
	return this->lambdas;
}

// Description: Returns lift coordinates
IntPair Field::GetLift()
{
	return this->lift;
}

// Description: Returns the state of the lift
bool Field::isLiftOpened()
{
	return this->liftIsOpen;
}

// Description: Updates map according to the rules
void Field::UpdateMap()
{
	// Creating new state to record changes on the map
	char ** newState = new _MineObject * [mapHeight];
	for (size_t i = 0; i < mapHeight; i++) {
		newState[i] = new _MineObject [mapWidth];
		for (size_t j = 0; j < mapWidth; j++) {
			newState[i][j] = WALL;
		}
	}
	
	for (size_t i = 1; i < mapHeight - 1; i++) {
		for (size_t j = 1; j < mapWidth - 1; j++) {
			// If (x; y) contains a Rock, and (x; y-1) is Empty:
			// (x; y) is updated to Empty, (x; y-1) is updated to Rock.
			if (map[i][j] == STONE && map[i + 1][j] == EMPTY) {
				newState[i][j] = EMPTY;
				newState[i + 1][j] = STONE;
				if (map[i + 2][j] == ROBOT)
					robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Rock, (x+1; y) is Empty and (x+1; y-1) is Empty:
			// (x; y) is updated to Empty, (x+1; y-1) is updated to Rock.
			else if (map[i][j] == STONE && map[i + 1][j] == STONE
				&& map[i][j + 1] == EMPTY && map[i + 1][j + 1] == EMPTY) {
					newState[i][j] = EMPTY;
					newState[i + 1][j + 1] = STONE;
					if (map[i + 2][j + 1] == ROBOT) 
						robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Rock, either (x+1; y) is not Empty
			// or (x+1; y-1) is not Empty, (x-1; y) is Empty and (x-1; y-1) is Empty:
			// (x; y) is updated to Empty, (x-1; y-1) is updated to Rock.
			else if (map[i][j] == STONE && map[i + 1][j] == STONE
				&& (map[i][j + 1] != EMPTY || map[i + 1][j + 1] != EMPTY)
				&& map[i][j - 1] == EMPTY && map[i + 1][j - 1] == EMPTY) {
					newState[i][j] = EMPTY;
					newState[i + 1][j - 1] = STONE;
					if (map[i + 2][j - 1] == ROBOT) 
						robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Lambda, (x+1; y) is Empty and (x+1; y-1) is Empty:
			// (x; y) is updated to Empty, (x+1; y-1) is updated to Rock.
			else if (map[i][j] == STONE && map[i + 1][j] == LAMBDA
				&& map[i][j + 1] == EMPTY && map[i + 1][j + 1] == EMPTY) {
					newState[i][j] = EMPTY;
					newState[i + 1][j + 1] = STONE;
					if (map[i + 2][j + 1] == ROBOT) 
						robotIsDead = true;
			}
			// In all other cases, (x; y) remains unchanged.
		}
	}
	// If (x; y) contains a Closed Lambda Lift, and there are no Lambdas remaining:
	// (x; y) is updated to Open Lambda Lift.
	if (lambdas.empty()) {
		SetLiftState(true);
		newState[lift.first][lift.second] = OPENED_LIFT;
	}

	// Rewriting old map according to the new state
	for (size_t i = 0; i < mapHeight; i++) {
		for (size_t j = 0; j < mapWidth; j++) {
			if (newState[i][j] != WALL) map[i][j] = newState[i][j];
		}
	}

	// Freeing memory
	for (size_t i = 0; i < mapHeight; i++)
		delete [] newState[i];
}

// Description: Checks, whether robot can go on this cage or not
bool Field::isWalkable(size_t x, size_t y)																						// TBD: add some euristic
{
	// If there is a wall, then robot fails
	if (map[x][y] == WALL) return false;
	// On the other side, robot can't go on right or left cage concerning him
	// if there is a stone in this cage and there is something in next cage
	if (map[x][y] == STONE) {											// If there is a stone in this cage:
		if (x == robot.first && y - 1 == robot.second) {			// then, if robot is to the left of a cage
			if (map[x][y + 1] == EMPTY) return true;				// then robot succeeds if the right cage near stone is empty
		} else if (x == robot.first && y + 1 == robot.second) {		// otherwise, if robot is to the right of a cage
			if (map[x][y - 1] == EMPTY) return true;				// then robot succeeds if the left cage near stone is empty.
		}
		return false;												// Robot fails in all other cases (i.e. next cage isn't empty).
	}
	// If there is a closed lift, then robot fails
	if (map[x][y] == CLOSED_LIFT) return false;
	// Robot succeeds in all other cases.
	// I.e. there is an earth, lambda or an open lift in the cage or the cage is empty.
	return true;
}

Field Field::operator = (const Field & field)
{
	mapWidth = field.mapWidth;
	mapHeight = field.mapHeight;
	robot = field.robot;
	lambdas = field.lambdas;
	lift = field.lift;
	liftIsOpen = field.liftIsOpen;

	map = new _MineObject * [field.mapHeight];
	for (size_t i = 0; i < field.mapHeight; i++) {
		map[i] = new _MineObject [field.mapWidth];
		for (size_t j = 0; j < field.mapWidth; j++) {
			map[i][j] = field.map[i][j];
		}
	}

	return *this;
}
