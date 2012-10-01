#include "Field.h"

Field::Field(void)
{
	mapWidth = -1;
	mapHeight = -1;
	map = NULL;
	liftIsOpen = false;
}

Field::Field(const Field & field)
{
	mapWidth = field.mapWidth;
	mapHeight = field.mapHeight;
	robot = field.robot;
	lambdas = field.lambdas;
	lift = field.lift;
	liftIsOpen = field.liftIsOpen;

	map = new char * [mapHeight];
	for (int i = 0; i < mapHeight; i++) {
		map[i] = new char [mapWidth];
		for (int j = 0; j < mapWidth; j++) {
			map[i][j] = field.map[i][j];
		}
	}
}

Field::~Field(void)
{
	if (map)
		for (int i = 0; i < mapHeight; i++)
			delete [] map[i];
}

// Description: Loads map from file and fills Field's fields =)
int Field::LoadMap(char * file)
{
	if (file == NULL) return -1;
	ifstream fin(file);
	if (!fin.is_open()) return -2;

	vector<string> buf;
	string str;
	int width = 0;

	// Counting mine's dimension
	while (!fin.eof()) {
		getline(fin, str);
		buf.push_back(str);
		if (str.size() > width) width = str.size(); // remember the longest row
	}

	mapWidth = width;
	mapHeight = buf.size();
	fin.seekg(0, ios::beg);

	map = new char * [mapHeight];
	for (int i = 0; i < mapHeight; i++) {
		map[i] = new char [mapWidth];
		for (int j = 0; j < mapWidth; j++) {

			// All rows shorter than map width are supplemented with whitespaces
			if (j >= buf.at(i).size()) {
				map[i][j] = ' ';
				continue;
			}
			map[i][j] = buf.at(i)[j];

			if (map[i][j] == 'R') {				// remembering robot coordinates
				robot.first = i;
				robot.second = j;
			} else if (map[i][j] == '\\')
				lambdas.push_back(pair<int, int>(i, j));	// filling lambdas's list
			else if (map[i][j] == 'L') {		// remembering closed lift coordinates
				lift.first = i;
				lift.second = j;
			} else if (map[i][j] == 'O') {		// remembering open lift coordinates
				lift.first = i;
				lift.second = j;
				liftIsOpen = true;
			}
		}
	}

	return 0;
}

// Description: Prints map using the specified stream
void Field::saveMap(ostream &sout)
{
	sout << endl;
	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapWidth; j++) {
			sout << map[i][j];
		}
		sout << endl;
	}
	sout << endl;
}

// Description: Changes robot coordinates
void Field::SetRobot(int x, int y)
{
	robot.first = x;
	robot.second = y;
}

void Field::SetLiftState(bool isOpen)
{
	liftIsOpen = isOpen;
}

void Field::ReorderLambdas(vector<int> order)
{
	//vector<pair<int, int>> tmp;
	//for (int i = 0; i < order.size(); i++) {
	//	tmp.push_back(lambdas.at(order.at(i)));
	//}
	//lambdas.clear();
	//for (int i = 0; i < tmp.size(); i++) {
	//	lambdas.push_back(tmp.at(i));
	//}
}

void Field::ClearLambdas()
{
	lambdas.clear();
}

void Field::AddLambda(pair<int, int> lambda)
{
	lambdas.push_back(lambda);
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
pair<int, int> Field::GetRobot()
{
	return this->robot;
}

// Description: Returns list of lambda's coordinates for all lambdas on map
vector<pair<int, int>> Field::GetLambdas()
{
	return this->lambdas;
}

// Description: Returns lift coordinates
pair<int, int> Field::GetLift()
{
	return this->lift;
}

// Description: Returns the state of the lift
bool Field::isLiftOpened()
{
	return this->liftIsOpen;
}

// Description: Checks, whether robot can go on this cage or not
bool Field::isWalkable(int x, int y)																						// TBD: add some euristic
{
	// If there is a wall, then robot fails
	if (map[x][y] == '#') return false;
	// On the other side, robot can't go on right or left cage concerning him
	// if there is a stone in this cage and there is something in next cage
	if (map[x][y] == '*') {											// If there is a stone in this cage:
		if (x == robot.first && y - 1 == robot.second) {			// then, if robot is to the left of a cage
			if (map[x][y + 1] != ' ') return false;					// then robot fails if the right cage near stone isn't empty
		} else if (x == robot.first && y + 1 == robot.second) {		// otherwise, if robot is to the right of a cage
			if (map[x][y - 1] != ' ') return false;					// then robot fails if the left cage near stone isn't empty.
		} else return true;											// Robot succeeds in all other cases (i.e. next cage is empty).
	}
	// If there is a closed lift, then robot fails
	if (map[x][y] == 'L') return false;
	// Robot succeeds in all other cases.
	// I.e. there is an earth, lambda or an open lift in the cage or the cage is empty.
	return true;
}