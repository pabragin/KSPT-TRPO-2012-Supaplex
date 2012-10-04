#include "Simulator.h"


Simulator::Simulator(Field & amine)
{
	this->mine = amine;
	robotIsDead = false;
}


Simulator::~Simulator(void)
{
}

vector<pair<int, int>> Simulator::GetPath()
{
	return this->path;
}

void Simulator::StartSimulation(vector<pair<int, int>> waypoints)
{
	Field f = mine;


	mine.ClearLambdas();
	for (int i = waypoints.size() - 1; i > 0; i--) {							// waypoint #0 is a Robot !!!
		mine.AddLambda(waypoints.at(i));
	}

	int result;
	missedLambdas.clear();

	//mine.SaveMap(cout);														// testing print to stdout

	for (int i = mine.GetLambdas().size() - 1; i >= 0; i--) {					// waypoint #0 is a Robot !!!
		if (FindUnexpectedLambda(i)) {
			mine.PopBackLambda();
			continue;
		}

		MakeSnapshot();

		//cout << "Saved global snapshot:" << endl;
		//snapshot.back().SaveMap(cout);

		result = MoveRobot(mine.GetLambdas().at(i));

		if (result == -1) {
			LoadSnapshot();

			//cout << "Loaded global snapshot:" << endl;
			//mine.SaveMap(cout);

			missedLambdas.push_back(mine.GetLambdas().at(i));
		}

		mine.PopBackLambda();
	}


	//mine = f;
	//for (int i = 0; i < path.size(); i++) {
	//	Step(path[i].first, path[i].second);
	//	UpdateMap();
	//	mine.SaveMap(cout);
	//}
}

// Description: Updates map according to the rules
void Simulator::UpdateMap()
{
	// Creating new state to record changes on the map
	char ** newState = new char* [mine.GetHeight()];
	for (int i = 0; i < mine.GetHeight(); i++) {
		newState[i] = new char [mine.GetWidth()];
		for (int j = 0; j < mine.GetWidth(); j++) {
			newState[i][j] = '#';
		}
	}
	
	for (int i = 1; i < mine.GetHeight() - 1; i++) {
		for (int j = 1; j < mine.GetWidth() - 1; j++) {
			// If (x; y) contains a Rock, and (x; y-1) is Empty:
			// (x; y) is updated to Empty, (x; y-1) is updated to Rock.
			if (mine.GetMap()[i][j] == '*' && mine.GetMap()[i + 1][j] == ' ') {
				newState[i][j] = ' ';
				newState[i + 1][j] = '*';
				if (mine.GetMap()[i + 2][j] == 'R') 
					robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Rock, (x+1; y) is Empty and (x+1; y-1) is Empty:
			// (x; y) is updated to Empty, (x+1; y-1) is updated to Rock.
			else if (mine.GetMap()[i][j] == '*' && mine.GetMap()[i + 1][j] == '*'
				&& mine.GetMap()[i][j + 1] == ' ' && mine.GetMap()[i + 1][j + 1] == ' ') {
					newState[i][j] = ' ';
					newState[i + 1][j + 1] = '*';
					if (mine.GetMap()[i + 2][j + 1] == 'R') 
						robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Rock, either (x+1; y) is not Empty
			// or (x+1; y-1) is not Empty, (x-1; y) is Empty and (x-1; y-1) is Empty:
			// (x; y) is updated to Empty, (x-1; y-1) is updated to Rock.
			else if (mine.GetMap()[i][j] == '*' && mine.GetMap()[i + 1][j] == '*'
				&& (mine.GetMap()[i][j + 1] != ' ' || mine.GetMap()[i + 1][j + 1] != ' ')
				&& mine.GetMap()[i][j - 1] == ' ' && mine.GetMap()[i + 1][j - 1] == ' ') {
					newState[i][j] = ' ';
					newState[i + 1][j - 1] = '*';
					if (mine.GetMap()[i + 2][j - 1] == 'R') 
						robotIsDead = true;
			}
			// If (x; y) contains a Rock, (x; y-1) contains a Lambda, (x+1; y) is Empty and (x+1; y-1) is Empty:
			// (x; y) is updated to Empty, (x+1; y-1) is updated to Rock.
			else if (mine.GetMap()[i][j] == '*' && mine.GetMap()[i + 1][j] == '\\'
				&& mine.GetMap()[i][j + 1] == ' ' && mine.GetMap()[i + 1][j + 1] == ' ') {
					newState[i][j] = ' ';
					newState[i + 1][j + 1] = '*';
					if (mine.GetMap()[i + 2][j + 1] == 'R') 
						robotIsDead = true;
			}
			// In all other cases, (x; y) remains unchanged.
		}
	}
	// If (x; y) contains a Closed Lambda Lift, and there are no Lambdas remaining:
	// (x; y) is updated to Open Lambda Lift.
	if (mine.GetLambdas().size() == 1 && mine.GetLambdas().back() == mine.GetLift() && missedLambdas.empty()) {
		mine.SetLiftState(true);
		newState[mine.GetLift().first][mine.GetLift().second] = 'O';
	}

	// Rewriting old map according to the new state
	for (int i = 0; i < mine.GetHeight(); i++) {
		for (int j = 0; j < mine.GetWidth(); j++) {
			if (newState[i][j] != '#') mine.GetMap()[i][j] = newState[i][j];
		}
	}

	// Freeing memory
	for (int i = 0; i < mine.GetHeight(); i++)
		delete [] newState[i];
}


bool Simulator::IsLiftBlocked() { //is lift blocked? lets see (will return true or false)
    
    int rob_x = mine.GetRobot().first;     //some varibles
	int rob_y = mine.GetRobot().second;
    int lift_x = mine.GetLift().first;
    int lift_y = mine.GetLift().second;
    // set of cases when the lift is blocked 
    if ((rob_x == lift_x)&&(rob_y-2 == lift_y)&&!(mine.GetMap()[rob_x+1][rob_y+2] == ' ')){
        return true;
    }
    if ((rob_x == lift_x)&&(rob_y+2 == lift_y)&&!(mine.GetMap()[rob_x+1][rob_y-2] == ' ')){
        return true;
    }
    if (((mine.GetMap()[rob_x][rob_y-2] == '*')||(mine.GetMap()[rob_x][rob_y-2] == '#'))&& (rob_x+1 == lift_x)&&(rob_y-1 == lift_y)){
        return true;
    }
    if (((mine.GetMap()[rob_x][rob_y+2] == '*')||(mine.GetMap()[rob_x][rob_y+2] == '#'))&&((rob_x+1 == lift_x)&&(rob_y+1 == lift_y))){
        return true;
    }
    //ban drop stones on the lift
    if(((lift_y == rob_y-1)||(lift_y == rob_y+1))&& (!(mine.GetMap()[lift_x-1][lift_y+1] == ' ')||!(mine.GetMap()[lift_x-1][lift_y-1] == ' '))){
        for (int i=1 ;i < (lift_x-rob_x-1);i++){
             if (mine.GetMap()[rob_x+i][rob_y-1] == ' '){
                  liftBlocked = true;
              }
             else {
                 liftBlocked = false;
             }
       } return liftBlocked;
    }
    else {
        return false;
    }
}

// Using A star algorithm modified for taking care about dynamic changes on map
// At the begining - add reaction on the robot death
int Simulator::MoveRobot(pair<int, int> target) {

	const int infinity = 1000000;	// infinity Hcost of the cell where robot dies
	int numberOfClosedListItems = 0;
	OpenListItem * closedList;	// array holding closed list items

	int startX = mine.GetRobot().first;
	int startY = mine.GetRobot().second;


	vector<pair<int, int>> resultPath;			// vector of coordinates of cells in found path
	int result = 0;
	const int nonexistent = 0, found = 1;		// path-related constants
	const int inOpenList = 1, inClosedList = 2;	// lists-related constants
	int parentX, parentY, Gcost;
	int ** whichList;			// used to record whether a cell is on the open list or on the closed list.
	pair<int, int> ** parent;	// used to record parent of each cage
	OpenListItem * openList;	// array holding open list items, which is maintained as a binary heap.
	int numberOfOpenListItems;

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

	parent = new pair<int, int>* [mine.GetHeight() + 1];
	for (int i = 0; i < mine.GetHeight(); i++) {
		parent[i] = new pair<int, int> [mine.GetWidth() + 1];
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
	parent[startX][startY] = pair<int, int> (startX, startY);


// ****************************

	cellsnapshot[startX][startY] = mine;	// first snapshot at start point
	
// ****************************


// 3. Do it until the path is found or recognized as nonexistent.

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
				bool stoneMoved = Step(openList[1].GetX(), openList[1].GetY());
				if(stoneMoved){
					if (IsLiftBlocked()) {
						openList[1].SetHcost(infinity);
						whichList[parentX][parentY] = inClosedList;                   // add item to the closed list
						DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems); // delete this item from the open list
						mine = cellsnapshot[ parent[parentX][parentY].first ] [ parent[parentX][parentY].second ];
						continue;
					}
			    }
				UpdateMap();

				//mine.SaveMap(cout);																		// testing print to stdout
			}

			// Step is made - we can check new position
			// 
			// Cheking robot's death after update (this is a simple algorithm, need to add more euristic methods)
			if (robotIsDead) {

				// If this cell is target cell, then we refuse it at all and roll back
				if (openList[1].GetX() == target.first && openList[1].GetY() == target.second) {
					result = nonexistent;
					break;
				}

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
			resultPath.push_back(pair<int, int> (x, y));

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
		
	} else return -1;

// 7. Freeing used memory

	for (int i = 0; i < mine.GetHeight(); i++) {
		delete [] whichList[i];
		delete [] parent[i];
		delete [] cellsnapshot[i];
	}
	delete [] openList;

	delete [] closedList;

	return 0;
}


int Simulator::FindMissedLambda(pair<int, int> lambda)
{
	for (int i = 0; i < missedLambdas.size(); i++) {										// this is the worst method!!!
		if (missedLambdas.at(i) == lambda) return i;
	}
	return -1;
}
bool Simulator::FindUnexpectedLambda(int index)
{
	for (int i = 0; i < unexpectedLambdas.size(); i++) {									// this is the worst method!!!
		if (unexpectedLambdas.at(i) == index) return true;
	}
	return false;
}

// Description: Moves robot to the target cell
bool Simulator::Step(int x, int y)
{
    bool stoneMoved = false;
	int xold = mine.GetRobot().first;
	int yold = mine.GetRobot().second;
    
	// If there is a stone in this cage
	if (mine.GetMap()[x][y] == '*') {
		if (x == xold && y - 1 == yold) {				// then, if robot is to the left of the stone
			mine.GetMap()[x][y + 1] = '*';
		} else if (x == xold && y + 1 == yold) {		// otherwise, if robot is to the right of the stone
			mine.GetMap()[x][y - 1] = '*';
		}
        stoneMoved = true;
	}

	mine.GetMap()[xold][yold] = ' ';
	mine.GetMap()[x][y] = 'R';
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
	int parentX, int parentY, int Gcost, int ** whichList, pair<int, int> ** parent, pair<int, int> target, 
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

					if ( !(x == parentX + 1 && y == parentY && mine.GetMap()[parentX - 1][parentY] == '*') ) {

						// If cell is not already on the open list and is not in the closed list, add it to the open list.
						if (whichList[x][y] != inOpenList && whichList[x][y] != inClosedList) {
							numberOfOpenListItems++;					// increment number of items in the heap
							openList[numberOfOpenListItems].SetX(x);	// record the x and y coordinates of the new item
							openList[numberOfOpenListItems].SetY(y);

							// Figure out its G cost
							openList[numberOfOpenListItems].SetGcost(Gcost + 1);
									
							// Figure out its H and F costs and parent
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
								parent[x][y].first = parentX;			// change the cell's parent
								parent[x][y].second = parentY;
								openList[index].SetGcost(Gcost);	// change the G cost
								openList[index].CalculateFcost();	// update the F cost
								BubbleItemInBinaryHeap(openList, index);		// update cell's position on the open list
							}
						}
						// If cell is already on the open list and it is not current cell's parent, choose better G and F costs.
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
						}
					}//
				}
			}
		}
	}
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