#include "Simulator.h"


Simulator::Simulator(Field & amine)
{
	this->mine = amine;
	robotIsDead = false;
}


Simulator::~Simulator(void)
{
}

vector<IntPair> Simulator::GetPath()
{
	return this->path;
}

void Simulator::StartSimulation(vector<IntPair> waypoints)
{
	//cout << "Lambdas: " << waypoints.size() - 2 << endl;

	mine.ClearLambdas();
	for (int i = waypoints.size() - 1; i > 0; i--) {							// waypoint #0 is a Robot !!!
		mine.AddLambda(waypoints.at(i));
	}

	int result;
	missedLambdas.clear();

	for (int i = mine.GetLambdas().size() - 1; i >= 0; i--) {					// waypoint #0 is a Robot !!!
		if (FindUnexpectedLambda(i)) {
			mine.PopBackLambda();
			continue;
		}

		MakeSnapshot();

		//cout << "Saved global snapshot:" << endl;
		//snapshot.back().SaveMap(cout);
		//size_t index = path.size();

		result = MoveRobotToTarget(mine.GetLambdas().at(i));

		if (result == 0) {
			if (snapshot.empty()) break;
			
			LoadSnapshot();

			//cout << "Loaded global snapshot:" << endl;
			//mine.SaveMap(cout);

			missedLambdas.push_back(mine.GetLambdas().at(i));
			continue;
		}

		mine.PopBackLambda();
		
	}

	//int n = mine.GetLambdas().size();
	//bool finished = true;
	//if (n > 0) {
	//	n--;
	//	finished = false;
	//}
	//cout << endl;
	//cout << "Map is finished: " << finished << endl;
	//cout << "Lambdas left: " << n << endl;
	//cout << "Moves: " << this->path.size() - 1 << endl;

	//mine.SaveMap(cout);
}

// Description: Updates map according to the rules
void Simulator::UpdateMap()
{
	mine.UpdateMap();

	// If (x; y) contains a Closed Lambda Lift, and there are no Lambdas remaining:
	// (x; y) is updated to Open Lambda Lift.
	if (mine.GetLambdas().size() == 1 && mine.GetLambdas().back() == mine.GetLift() && missedLambdas.empty()) {
		mine.SetLiftState(true);
		mine.SetObject(mine.GetLift().first, mine.GetLift().second, OPENED_LIFT);
	}
}


bool Simulator::IsLiftBlocked() {			// is lift blocked? lets see (will return true or false)
    int robotX = mine.GetRobot().first;     // some variables for sintax sugar
	int robotY = mine.GetRobot().second;
    int liftX = mine.GetLift().first;
	int liftY = mine.GetLift().second;

	if (!mine.isWalkable(liftX - 1, liftY) && !mine.isWalkable(liftX + 1, liftY) && 
		!mine.isWalkable(liftX, liftY - 1) && !mine.isWalkable(liftX, liftY + 1)) {
			return true;
	}

	int stoneX = robotX, stoneY;
	if (mine.GetObject(robotX, robotY - 1) == STONE)
		stoneY = robotY - 1;
	else if (mine.GetObject(robotX, robotY + 1) == STONE)
		stoneY = robotY + 1;
	
	int endX = -1;
	for (int i = stoneX + 1; i <= liftX; i++) {
		if (mine.GetObject(i, stoneY) != EMPTY) {
			endX = i;
			break;
		}
	}
	if (endX == -1) return false;
	if ((endX == liftX && abs(stoneY - liftY) == 1) ||
		(endX == liftX - 1 && stoneY == liftY)) {
			return true;
	}

	return false;
}

// Using A star algorithm modified for taking care about dynamic changes on map
// At the begining - add reaction on the robot death
int Simulator::MoveRobotToTarget(IntPair target) {

	const int infinity = 1000000;	// infinity Hcost of the cell where robot dies
	int numberOfClosedListItems = 0;
	OpenListItem * closedList;	// array holding closed list items

	int startX = mine.GetRobot().first;
	int startY = mine.GetRobot().second;


	vector<IntPair> resultPath;			// vector of coordinates of cells in found path
	int result = 0;
	const int nonexistent = 0, found = 1;		// path-related constants
	const int inClosedList = 2;	// lists-related constants
	int parentX, parentY, Gcost;
	int ** whichList;			// used to record whether a cell is on the open list or on the closed list.
	IntPair ** parent;	// used to record parent of each cage
	OpenListItem * openList;	// array holding open list items, which is maintained as a binary heap.
	int numberOfOpenListItems = 0;

// ****************************

	Field ** cellsnapshot;	// array holding current field state for each cell (when robot stays on it)

// ****************************

// 1. Allocate memory and reset variables

	whichList = new int* [mine.GetHeight() + 1];
	for (int i = 0; i < mine.GetHeight(); i++) {
		whichList[i] = new int [mine.GetWidth() + 1];
		for (int j = 0; j < mine.GetWidth(); j++)
			whichList[i][j] = 0;
	}

	parent = new IntPair * [mine.GetHeight() + 1];
	for (int i = 0; i < mine.GetHeight(); i++) {
		parent[i] = new IntPair [mine.GetWidth() + 1];
	}

	openList = new OpenListItem [mine.GetWidth()*mine.GetHeight()+2];


// ****************************

	closedList = new OpenListItem [mine.GetWidth()*mine.GetHeight()+2];
	
	cellsnapshot = new Field* [mine.GetHeight() + 1];
	for (int i = 0; i < mine.GetHeight(); i++) {
		cellsnapshot[i] = new Field [mine.GetWidth() + 1];
	}
	
// ****************************


// 2. Add the starting cell to the open list.
	
	numberOfOpenListItems = 1;
	openList[1].SetX(startX);	// assign it as the top item in the binary heap
	openList[1].SetY(startY);
	openList[1].SetGcost(0);	// reset starting cell's G value to 0
	parent[startX][startY] = IntPair (startX, startY);
	

// ****************************

	cellsnapshot[startX][startY] = mine;	// first snapshot at start point
	
// ****************************


// 3. Do it until the path is found or recognized as nonexistent.


	/*
	result = Step(IntPair(startX, startY), 0, openList, numberOfOpenListItems, whichList, parent, 
			target, closedList, numberOfClosedListItems);

	if (result == found) {
		// Working backwards from the target to the start by checking each cell's parent.
		int x = target.first, y = target.second;
		IntPair tmp;
		while (true) {
			// Save path in reverse order
			resultPath.push_back(IntPair (x, y));

			if (x == startX && y == startY) break;

			// Look up the parent of the current cell
			tmp = parent[x][y].front();
			parent[x][y].erase(parent[x][y].begin());
			y = tmp.second;
			x = tmp.first;
		} 

		// Saving founded path
		for (int i = resultPath.size() - 2; i >= 0; i--) {
			path.push_back(resultPath[i]);

			int index = mine.FindLambda(path.back());
			if (index == -1) index = FindMissedLambda(path.back());
			if (index != -1) unexpectedLambdas.push_back(index);
		}
		
	} //else return -1;

	*/


	


// 3.0 Analyze target cell's position

	// Trying to avoid situations like this:
	//
	//  *
	// #\#
	//
	// and this:
	//
	// #*
	// #.
	// # #
	// # #
	// #\#
	// ###
	// 

	// IsDeadLock(target.first, target.second);

	while (true) {
	
// 3.1. If the open list is not empty, take the first cell off of the list (i.e. the lowest F cost cell).

		if (numberOfOpenListItems != 0) {

			// record cell coordinates and Gcost of the item as parent for adjacent cells (see below)
			parentX = openList[1].GetX();
			parentY = openList[1].GetY();
			Gcost = openList[1].GetGcost();


// ***************************

			// If it is not the start cell
			if (openList[1].GetX() != startX || openList[1].GetY() != startY) {
				// loading field state relating to this cell's parent (from which robot makes a step to this cell)
				mine = cellsnapshot[ parent[parentX][parentY].first ] [ parent[parentX][parentY].second ];
				// making a step and updating map
				bool stoneMoved = MoveRobot(openList[1].GetX(), openList[1].GetY());

				if (stoneMoved && IsLiftBlocked()) {
					openList[1].SetHcost(infinity);
					whichList[parentX][parentY] = inClosedList;                   // add item to the closed list
					DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems); // delete this item from the open list
					mine = cellsnapshot[ parent[parentX][parentY].first ] [ parent[parentX][parentY].second ];
					continue;
			    }

				UpdateMap();

				//mine.SaveMap(cout);																		// testing print to stdout
			}

			// Step is made - we can check new position
			// 
			// Cheking robot's death after update (this is a simple algorithm, need to add more euristic methods)
			if (robotIsDead) {

				//// If this cell is target cell, then we refuse it at all and roll back
				//if (openList[1].GetX() == target.first && openList[1].GetY() == target.second) {					// TBD: useless?
				//	result = nonexistent;
				//	break;
				//}

				// If it is not our target cell, set infinity H cost and transfer item to the closed list - it is not the best rule
				openList[1].SetHcost(infinity);
				whichList[parentX][parentY] = inClosedList;						// add item to the closed list
				numberOfClosedListItems++;
				closedList[numberOfClosedListItems] = openList[1];
				DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems);	// delete this item from the open list

				mine = cellsnapshot[ parent[parentX][parentY].first ] [ parent[parentX][parentY].second ];	// load snapshot

				//cout << "Back:" << endl;
                //mine.SaveMap(cout);																		// testing print to stdout

				robotIsDead = false;
				continue;
			}

			if (openList[1].GetX() == target.first && openList[1].GetY() == target.second) {
				result = found;
				break;
			}

			cellsnapshot[parentX][parentY] = mine;

// ***************************

			whichList[parentX][parentY] = inClosedList;						// add item to the closed list
			numberOfClosedListItems++;
			closedList[numberOfClosedListItems] = openList[1];
			DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems);	// delete this item from the open list

// 3.2. Check the adjacent squares and add them to the open list

			AddAdjacentCellsToOpenList(openList, numberOfOpenListItems, parentX, parentY, Gcost,
									   whichList, parent, target, closedList, numberOfClosedListItems);

			//cout << numberOfOpenListItems << endl;
			//for (int k = 1; k < numberOfOpenListItems + 1; k++)
			//	cout << openList[k].GetX() << ":" << openList[k].GetY() << " F: " << openList[k].GetFcost() << " G: " << openList[k].GetGcost() << endl;

		} else {

// 3.3. If open list is empty then there is no path.	
		
			result = nonexistent;
			break;
		}

	}	// end while

// 6. Save the path if it exists.

	if (result == found) {

// 6.1. Working backwards from the target to the start by checking each cell's parent.

		int x = target.first, y = target.second;
		int tmp;
		while (true) {
			// Save path in reverse order
			resultPath.push_back(IntPair (x, y));

			if (x == startX && y == startY) break;

			// Look up the parent of the current cell
			tmp = parent[x][y].first;
			y = parent[x][y].second;
			x = tmp;
		} 

// 6.2. Saving founded path

		for (int i = resultPath.size() - 2; i >= 0; i--) {
			path.push_back(resultPath[i]);

			int index = mine.FindLambda(path.back());
			if (index == -1) index = FindMissedLambda(path.back());
			if (index != -1) unexpectedLambdas.push_back(index);
		}
		
	} //else return -1;

// 7. Freeing used memory

	for (int i = 0; i < mine.GetHeight(); i++) {
		delete [] whichList[i];
		delete [] parent[i];
		delete [] cellsnapshot[i];
	}
	delete [] openList;

	delete [] closedList;

	return result;
}

// Magic.
int Simulator::Step(IntPair cell, int Gcost, OpenListItem * openList, int & numberOfOpenListItems, int ** whichList, 
	IntPair ** parent, IntPair target, OpenListItem * closedList, int & numberOfClosedListItems)
{
	const int nonexistent = 0, found = 1;
	int result = nonexistent;
	IntPair nextCell(-1, -1);

	//const int infinity = 1000000;
	const int inClosedList = 2;	// lists-related constants
	int parentX, parentY;

	// record cell coordinates and Gcost of the item as parent for adjacent cells (see below)
	parentX = cell.first;
	parentY = cell.second;

	OpenListItem * currOpenList = new OpenListItem [mine.GetWidth()*mine.GetHeight()+2];
	int currNumberOfOpenedItems;
	OpenListItem * currClosedList = new OpenListItem [mine.GetWidth()*mine.GetHeight()+2];
	int currNumberOfClosedItems;

	Field snapshot = mine;
	do {
		// Check the adjacent squares and add them to the open list
		AddAdjacentCellsToOpenList(openList, numberOfOpenListItems, parentX, parentY, Gcost,
									whichList, parent, target, closedList, numberOfClosedListItems);

		// making a step and updating map
		bool stoneMoved = MoveRobot(openList[1].GetX(), openList[1].GetY());
		UpdateMap();

		// Step was made - we can check new position
		// Cheking robot's death after update and lift blocking situations
		if (robotIsDead || (stoneMoved && IsLiftBlocked())) {
			DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems);	// delete this item from the open list
			mine = snapshot;	// load snapshot
			robotIsDead = false;
			continue;
		}

		if (openList[1].GetX() == target.first && openList[1].GetY() == target.second) {
			result = found;
			break;
		}

		whichList[parentX][parentY] = inClosedList;						// add item to the closed list
		numberOfClosedListItems++;
		closedList[numberOfClosedListItems] = openList[1];
		IntPair currCell = IntPair(openList[1].GetX(), openList[1].GetY());
		DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems);	// delete this item from the open list

		//copy(openList, openList + numberOfOpenListItems + 1, currOpenList);
		currNumberOfOpenedItems = numberOfOpenListItems;
		//copy(closedList, closedList + numberOfOpenListItems + 1, currClosedList);
		currNumberOfClosedItems = numberOfClosedListItems;
		int next = Step(currCell, Gcost + 1, currOpenList, currNumberOfOpenedItems, whichList, parent, 
			target, currClosedList, currNumberOfClosedItems);

		if (next == 0) {
			mine = snapshot;	// load snapshot
			robotIsDead = false;
		} else {
			result = found;
			break;
		}
	} while (numberOfOpenListItems != 0);
	
	return result;
}

bool Simulator::IsDeadLock(int x, int y)
{
	if (!mine.isWalkable(x, y - 1) && !mine.isWalkable(x, y + 1) && mine.GetObject(x - 1, y) == STONE)
		return true;

	for (int i = x - 1; i > 0; i--) {
		if (!mine.isWalkable(i, y - 1) && !mine.isWalkable(i, y + 1) && mine.isWalkable(i, y)) {
			continue;
		}
		if (mine.GetObject(i - 1, y) == STONE && mine.isWalkable(i, y))
			return true;
	}
	return false;
}

int Simulator::FindMissedLambda(IntPair lambda)
{
	for (size_t i = 0; i < missedLambdas.size(); i++) {										// this is the worst method!!!
		if (missedLambdas.at(i) == lambda) return i;
	}
	return -1;
}

bool Simulator::FindUnexpectedLambda(int index)
{
	for (size_t i = 0; i < unexpectedLambdas.size(); i++) {									// this is the worst method!!!
		if (unexpectedLambdas.at(i) == index) return true;
	}
	return false;
}

// Description: Moves robot to the target cell
bool Simulator::MoveRobot(int x, int y)
{
    bool stoneMoved = false;
	int xold = mine.GetRobot().first;
	int yold = mine.GetRobot().second;
    
	// If there is a stone in this cage
	if (mine.GetObject(x, y) == STONE) {
		if (x == xold && y - 1 == yold) {				// then, if robot is to the left of the stone
			mine.SetObject(x, y + 1, STONE);
		} else if (x == xold && y + 1 == yold) {		// otherwise, if robot is to the right of the stone
			mine.SetObject(x, y - 1, STONE);
		}
        stoneMoved = true;
	}

	mine.SetObject(xold, yold, EMPTY);
	mine.SetObject(x, y, ROBOT);
	mine.SetRobot(x, y);
    return stoneMoved;
}

// Description: Saves current mine state
void Simulator::MakeSnapshot()
{
	snapshot.push_back(mine);
}

// Description: Restores last mine state
void Simulator::LoadSnapshot()
{
	mine = snapshot.back();
	snapshot.pop_back();
}


void Simulator::AddAdjacentCellsToOpenList(OpenListItem * openList, int & numberOfOpenListItems,
	int parentX, int parentY, int Gcost, int ** whichList, IntPair ** parent, IntPair target, 
	OpenListItem * closedList, int & numberOfClosedListItems)
{
	const int inOpenList = 1, inClosedList = 2;	// lists-related constants
	int index;

	for (int x = parentX - 1; x <= parentX + 1; x++) {
		for (int y = parentY - 1; y <= parentY + 1; y++) {
			if ((x != parentX && y != parentY) || (x == parentX && y == parentY))
				continue;

			// If not off the map (avoiding array-out-of-bounds errors)
			if (x != -1 && y != -1 && x != mine.GetHeight() && y != mine.GetWidth()) {

				// If not a wall/obstacle square.
				if (mine.isWalkable(x, y)) {

					if ( !(x == parentX + 1 && y == parentY && mine.GetMap()[parentX - 1][parentY] == STONE) ) {

						// If cell is not already on the open list and is not in the closed list, add it to the open list.
						if (whichList[x][y] != inOpenList && whichList[x][y] != inClosedList) {
							numberOfOpenListItems++;					// increment number of items in the heap
							openList[numberOfOpenListItems].SetX(x);	// record the x and y coordinates of the new item
							openList[numberOfOpenListItems].SetY(y);

							// Figure out its G cost
							openList[numberOfOpenListItems].SetGcost(Gcost + 1);
									
							// Figure out its H and F costs and parent
							//parent[x][y].push_back(IntPair(parentX, parentY));				// change the cell's parent
							parent[x][y].first = parentX;							// change the cell's parent
							parent[x][y].second = parentY;
							// F cost includes H cost except when we want to use A* algorithm as Dijkstra's algorithm
							openList[numberOfOpenListItems].SetHcost( (abs(x - target.first) + abs(y - target.second)) );
							openList[numberOfOpenListItems].CalculateFcost();	// update the F cost
									
							// Move the new open list item to the proper place in the binary heap.
							BubbleItemInBinaryHeap(openList, numberOfOpenListItems);

							whichList[x][y] = inOpenList;	// Change whichList value.
						}
						// If cell is already on the open list, choose better G and F costs.
						else if (whichList[x][y] == inOpenList) {
							index = GetItemIndexFromBinaryHeapByCoord(openList, numberOfOpenListItems, x, y);
							Gcost += 1;	// Figure out the G cost of this possible new path

							// If this path is shorter (G cost is lower) then change the parent cell, G cost and F cost. 		
							if (Gcost < openList[index].GetGcost()) {
								parent[x][y].first = parentX;		// change the cell's parent
								parent[x][y].second = parentY;
								openList[index].SetGcost(Gcost);	// change the G cost
								openList[index].CalculateFcost();	// update the F cost
								BubbleItemInBinaryHeap(openList, index);		// update cell's position on the open list
							}
						}
						// If cell is already on the closed list and it is not current cell's parent, choose better G and F costs.
						else if (whichList[x][y] == inClosedList) {
							int oldParX = parent[parentX][parentY].first;
							int oldParY = parent[parentX][parentY].second;

							if (oldParX != x || oldParY != y) {
								Gcost += 1;	// Figure out the G cost of this possible new path

								OpenListItem item;
								int index = -1;
								for (int i = 0; i < numberOfClosedListItems; i++) {
									if (closedList[i].GetX() == x && closedList[i].GetY() == y) {
										item = closedList[i];
										index = i;
										break;
									}
								}
								
								if ((item.GetHcost() == 1000000 && item.GetGcost() != Gcost) || 
									(item.GetHcost() != 1000000 && Gcost <= item.GetGcost() + 1)) {
									// If this path is shorter (G cost is lower) then change the parent cell, G cost and F cost. 		
									parent[x][y].first = parentX;			// change the cell's parent
									parent[x][y].second = parentY;
									numberOfOpenListItems++;
									openList[numberOfOpenListItems].SetX(x);
									openList[numberOfOpenListItems].SetY(y);
									openList[numberOfOpenListItems].SetGcost(Gcost);	// change the G cost
									openList[numberOfOpenListItems].SetHcost( (abs(x - target.first) + abs(y - target.second)) );
									openList[numberOfOpenListItems].CalculateFcost();	// update the F cost
									BubbleItemInBinaryHeap(openList, numberOfOpenListItems);			// update cell's position on the open list

									whichList[x][y] = inOpenList;	// Change whichList value.

									// Remove from closed list
									closedList[index] = closedList[numberOfClosedListItems];	// move last item to slot #index
									numberOfClosedListItems--;
								}
							}
						} //if (whichList[x][y] == inClosedList)
					}
				} //if (mine.isWalkable(x, y))
			}
		} // for (y)
	} // for (x)
}



// Description: Deletes the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
void Simulator::DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength)
{
	heapLength--;								// decrease number of items in heap
	heap[1] = heap[heapLength + 1];				// move last item up to slot #1
	SinkItemInBinaryHeap(heap, heapLength, 1);	// move item to the properly position
	
}

// Description: Finds the item in the binary heap by cell's coordinates and returns its index.
int Simulator::GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y)
{
	int index = -1;
	for (int i = 1; i <= heapLength; i++) {
		if (heap[i].GetX() == x && heap[i].GetY() == y) {
			index = i;
			break;
		}
	}
	return index;
}

// Description: Bubbles target item to the properly position in binary heap
void Simulator::BubbleItemInBinaryHeap(OpenListItem * heap, int index)
{
	while (index != 1) {	// While item hasn't bubbled to the top
		// Swap items if child < parent.
		if (heap[index].GetFcost() <= heap[index/2].GetFcost()) {
			OpenListItem temp = heap[index/2];
			heap[index/2] = heap[index];
			heap[index] = temp;
			index = index/2;
		} else break;
	}
}

// Description: Sinks target item to the properly position in binary heap
void Simulator::SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index)
{
	int curr, next = 1;

	// Repeat the following until the new item in slot #1 sinks to its proper spot in the heap.
	while (true) {
		curr = next;		
		if (2*curr + 1 <= heapLength) {	// if both children exist
			// Check if the F cost of the parent is greater than each child and select the lowest one.
			if (heap[curr].GetFcost() >= heap[2*curr].GetFcost()) 
				next = 2*curr;
			if (heap[next].GetFcost() >= heap[2*curr + 1].GetFcost())
				next = 2*curr+1;		
		} else {
			if (2*curr <= heapLength) {	// if only child #1 exists
				// Check if the F cost of the parent is greater than child #1	
				if (heap[curr].GetFcost() >= heap[2*curr].GetFcost()) 
					next = 2*curr;
			}
		}

		if (curr != next) {	// if parent's F > one of its children, swap them
			OpenListItem temp = heap[curr];
			heap[curr] = heap[next];
			heap[next] = temp;
		} else break;		// otherwise, exit loop
	}
}