#include "Field.h"

Field::Field(void)
{
	mapWidth = -1;
	mapHeight = -1;
	map = NULL;
	liftIsOpen = false;
	robotIsDead = false;
}

Field::Field(size_t width, size_t height)
{
	mapWidth = width;
	mapHeight = height;
	liftIsOpen = false;
	robotIsDead = false;

	map = new char * [mapHeight];
	for (size_t i = 0; i < mapHeight; i++) {
		map[i] = new char [mapWidth];
		for (size_t j = 0; j < mapWidth; j++) {
			if (i == 1 && j == 1) {
				map[i][j] = ROBOT;
			} else if (i == mapHeight - 2 && j == mapWidth - 1) {
				map[i][j] = CLOSED_LIFT;
			} else if (i == 0 || i == mapHeight - 1 ||
					   j == 0 || j == mapWidth - 1) {
				map[i][j] = WALL;
			} else {
				map[i][j] = EMPTY;
			}
		}
	}

	robot.first = 1;
	robot.second = 1;
	lift.first = mapHeight - 2;
	lift.second = mapWidth - 1;
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
	mapWidth = -1;
	mapHeight = -1;
	map = NULL;
	liftIsOpen = false;
	robotIsDead = false;
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

// Description: Checks whether mine is correct or not
// Returns: 0 if mine is correct, -1 otherwise
int Field::CheckMine()
{
	for (size_t i = 0; i < mapHeight; i++) {
		for (size_t j = 0; j < mapWidth; j++) {
			if (map[i][j] != EMPTY) {
				if (map[i][j] != WALL && map[i][j] != CLOSED_LIFT) {
					return -1;
				} else break;
			}
		}

		if (i == 17) {
		}

		for (size_t k = 0; k < mapWidth; k++) {
			if (map[i][mapWidth - 1 - k] != EMPTY) {
				if (map[i][mapWidth - 1 - k] != WALL && map[i][mapWidth - 1 - k] != CLOSED_LIFT) {
					return -1;
				} else break;
			}
		}
	}

	for (size_t i = 0; i < mapHeight; i += mapHeight - 1) {
		size_t index = 0;
		while (map[i][index] == EMPTY) {
			index++;
			if (index == mapWidth && (i != 0 && i != mapHeight - 1))
				return -1;
		}
		for (size_t j = index; j < mapWidth; j++) {
			if (map[i][j] != WALL && map[i][j] != CLOSED_LIFT) {
				if (map[i][j] == EMPTY) {
					while (j < mapWidth) {
						if (map[i][j] != EMPTY)
							return -1;
						j++;
					}
					break;
				} else
					return -1;
			}
		}
	}

	int robot_num = 0, closed_lift_num = 0;
	for (size_t i = 0; i < mapHeight; i++) {
		for (size_t j = 0; j < mapWidth; j++) {
			if (map[i][j] == ROBOT) {
				robot_num++;
			} else if (map[i][j] == CLOSED_LIFT) {
				closed_lift_num++;
			} else if (map[i][j] != STONE && map[i][j] != WALL && map[i][j] != EARTH && 
				map[i][j] != LAMBDA && map[i][j] != EMPTY) {
					return -1;
			}
		}
	}

	if (robot_num != 1 || closed_lift_num != 1)
		return -1;

	return 0;
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

void Field::EraseLambda(IntPair lambda)
{
	int index = FindLambda(lambda);
	if (index != -1) lambdas.erase(lambdas.begin() + index);
}

int Field::FindLambda(IntPair lambda)
{
	int index = 0;
	vector<IntPair>::iterator itr = find(lambdas.begin(), lambdas.end(), lambda);
	if (itr == lambdas.end()) index = -1;
	else index = distance(lambdas.begin(), itr);

	return index;
}

_MineObject Field::GetObject(size_t x, size_t y)
{
	if (x < mapHeight && y < mapWidth)
		return map[x][y];
	else
		return '\0';
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

bool Field::IsRobotDead()
{
	return this->robotIsDead;
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
bool Field::isWalkable(int x, int y)																						// TBD: add some euristic
{
	// Avoid 'index out of bounds' situations
	if (x >= (int) mapHeight || y >= (int) mapWidth) return false;
	if (x < 0 || y < 0) return false;

	// If there is a wall, then robot fails
	if (map[x][y] == WALL) return false;
	// On the other side, robot can't go on right or left cage concerning him
	// if there is a stone in this cage and there is something in next cage
	if (map[x][y] == STONE) {										// If there is a stone in this cage:
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
