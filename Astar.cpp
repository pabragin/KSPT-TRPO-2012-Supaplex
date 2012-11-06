#include "Astar.h"


Astar::Astar(void)
{
	mapWidth = -1;
	mapHeight = -1;
}

Astar::Astar(char ** field, int width, int height)
{
	map = field;
	mapWidth = width;
	mapHeight = height;
}


Astar::~Astar(void)
{
	map = NULL;
	resultPath.clear();
}

// Description: Returns the shortest path found
CoordinatesVector Astar::GetResultPath()
{
	return this->resultPath;
}

// Description: Returns length of the result path
int Astar::GetPathLength()
{
	return this->resultPath.size();
}

// Description: Finds a path using A*.
int Astar::FindPath(int startX, int startY, int targetX, int targetY, bool useHcost)
{
	int path = 0;
	const int nonexistent = 0, found = 1;		// path-related constants
	const int inOpenList = 1, inClosedList = 2;	// lists-related constants
	int parentX, parentY, Gcost, index;
	int ** whichList;	// used to record whether a cell is on the open list or on the closed list.
	Coordinates ** parent;	// used to record parent of each cage
	OpenListItem * openList;	// array holding open list items, which is maintained as a binary heap.
	int numberOfOpenListItems;

// 1. Checking start and target cells to avoid misunderstandings.

	// If start and target cells are the same cell
	if (startX == targetX && startY == targetY)
		return found;

	// If target cell is unwalkable
	if (map[targetX][targetY] == '#')
		return nonexistent;

// 2. Allocate memory and reset variables

	whichList = new int* [mapHeight + 1];
	for (int i = 0; i < mapHeight; i++) {
		whichList[i] = new int [mapWidth + 1];
		for (int j = 0; j < mapWidth; j++)
			whichList[i][j] = 0;
	}

	parent = new Coordinates* [mapHeight + 1];
	for (int i = 0; i < mapHeight; i++) {
		parent[i] = new Coordinates [mapWidth + 1];
	}

	openList = new OpenListItem [mapWidth*mapHeight+2];

	resultPath.clear();

// 3. Add the starting cell to the open list.

	numberOfOpenListItems = 1;
	openList[1].SetX(startX);	// assign it as the top item in the binary heap
	openList[1].SetY(startY);
	openList[1].SetGcost(0);	// reset starting cell's G value to 0

// 4. Do it until the path is found or recognized as nonexistent.

	while (true) {
	
// 4.1. If the open list is not empty, take the first cell off of the list (i.e. the lowest F cost cell).

		if (numberOfOpenListItems != 0) {
			// record cell coordinates and Gcost of the item as parent for adjacent cells (see below)
			parentX = openList[1].GetX();
			parentY = openList[1].GetY();
			Gcost = openList[1].GetGcost();

			whichList[parentX][parentY] = inClosedList;	// add item to the closed list
			DeleteTopItemFromBinaryHeap(openList, numberOfOpenListItems);				// delete this item from the open list

// 4.2. Check the adjacent squares and add them to the open list

			for (int x = parentX - 1; x <= parentX + 1; x++) {
				for (int y = parentY - 1; y <= parentY + 1; y++) {
					if ((x != parentX && y != parentY) || (x == parentX && y == parentY))
						continue;

					// If not off the map (avoiding array-out-of-bounds errors)
					if (x != -1 && y != -1 && x != mapHeight && y != mapWidth) {
						
						// If not already on the closed list (items on the closed list have already been considered and can now be ignored).
						if (whichList[x][y] != inClosedList) {
							// If not a wall/obstacle square.
							if (map[x][y] != '#' && map[x][y] != '*') {																				// TBD: use isWalkable(...) method from Field class
								// If cell is not already on the open list, add it to the open list.
								if (whichList[x][y] != inOpenList) {
									numberOfOpenListItems++;					// increment number of items in the heap
									openList[numberOfOpenListItems].SetX(x);	// record the x and y coordinates of the new item
									openList[numberOfOpenListItems].SetY(y);

									// Figure out its G cost
									openList[numberOfOpenListItems].SetGcost(Gcost + 1);
									
									// Figure out its H and F costs and parent
									parent[x][y].first = parentX;							// change the cell's parent
									parent[x][y].second = parentY;
									// F cost includes H cost except when we want to use A* algorithm as Dijkstra's algorithm
									if (useHcost) openList[numberOfOpenListItems].SetHcost( (abs(x - targetX) + abs(y - targetY)) );
									openList[numberOfOpenListItems].CalculateFcost();	// update the F cost
									
									// Move the new open list item to the proper place in the binary heap.
									BubbleItemInBinaryHeap(openList, numberOfOpenListItems);

									whichList[x][y] = inOpenList;	// Change whichList value.
								}
								// If cell is already on the open list, choose better G and F costs.
								else {
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
							}
						}
					}
				}
			}
		} else {

// 4.3. If open list is empty then there is no path.	
		
			path = nonexistent;
			break;
		}  

// 5. If target cell is added to open list then path has been found.

		if (whichList[targetX][targetY] == inOpenList) {
			path = found;
			break;
		}
	}	// end while

// 6. Save the path if it exists.

	if (path == found) {

// 6.1. Working backwards from the target to the start by checking each cell's parent.

		int x = targetX, y = targetY;
		int tmp;
		while (true) {
			// Save path in reverse order
			resultPath.push_back(Coordinates (x, y));

			if (x == startX && y == startY) break;

			// Look up the parent of the current cell
			tmp = parent[x][y].first;
			y = parent[x][y].second;
			x = tmp;
		} 

// 6.2. Reversing path

		int size = (int) resultPath.size();
		int amount = size/2;
		Coordinates tmppair;
		for (int i = 0; i < amount; i++) {
			tmppair = resultPath[size - i - 1];
			resultPath[size - i - 1] = resultPath[i];
			resultPath[i] = tmppair;
		}
		
	}

// 7. Freeing used memory

	for (int i = 0; i < mapHeight; i++)
		delete [] whichList[i];
	for (int i = 0; i < mapHeight; i++)
		delete [] parent[i];
	delete [] openList;

	return path;
}


// Description: Deletes the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
void Astar::DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength)
{
	heapLength--;								// decrease number of items in heap
	heap[1] = heap[heapLength + 1];				// move last item up to slot #1
	SinkItemInBinaryHeap(heap, heapLength, 1);	// move item to the properly position
	
}

// Description: Finds the item in the binary heap by cell's coordinates and returns its index.
int Astar::GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y)
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
void Astar::BubbleItemInBinaryHeap(OpenListItem * heap, int index)
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
void Astar::SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index)
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
