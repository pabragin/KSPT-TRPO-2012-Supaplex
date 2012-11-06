#include "TSPSolver.h"


TSPSolver::TSPSolver(Field * amine)
{
	this->mine = amine;

	if (!mine->GetLambdas().empty()) {
		nodes.push_back(mine->GetRobot());
		for (int i = 0; i < (int) mine->GetLambdas().size(); i++) {
			nodes.push_back(mine->GetLambdas().at(i));
		}
		nodes.push_back(mine->GetLift());
	}
}

TSPSolver::~TSPSolver(void)
{
}

// Description: Returns result path as sequence of cells's coordinates
vector<IntPair> TSPSolver::GetTourPath()
{
	return this->path;
}

// Description: Returns path between Start and Target nodes
vector<IntPair> TSPSolver::GetPath(const int & start, const int & target)
{
	// Check node's order
	if (start < target)
		return this->pathMatrix[IntPair (start, target)];
	else {
		// pathMatrix contains paths from A to B, where A < B.
		// But path from A to B, where A > B, is path from B to A in reverse order.
		// So, we need to reverse vector

		vector<IntPair> resultPath;
		vector<IntPair> * ptr = & this->pathMatrix[IntPair (target, start)];
		int size = ptr->size();
		for (int i = size - 1; i >= 0; i--) {
			resultPath.push_back(ptr->at(i));
		}
		return resultPath;
	}
}

// Description: Returns nodes
vector<IntPair> TSPSolver::GetNodes()
{
	return this->nodes;
}

// Description: Returns tour, i.e. nodes order in path
vector<int> TSPSolver::GetTour()
{
	return this->tour;
}

// Description: Returns common tour length
int TSPSolver::GetTourDistance()
{
	return this->tourDistance;
}

// Description: Solves TSP problem
void TSPSolver::Solve(const int & iterations)
{
	//SetMatrixes();				// initialize distance and path matrixes
	CreateNearestNeighbourTour();	// create tour using NN algorithm

	//for (int i = 0; i < tour.size(); i++) {
	//	cout << tour[i] << " ";
	//}
	//cout << endl;


	for (int i = 0; i < iterations; i++)
		StartTwoOpt();					// optimize the found tour

	//for (int i = 0; i < tour.size(); i++) {
	//	cout << tour[i] << " ";
	//}
	//cout << endl;

	//SetTourPath();					// build result path as sequence of cells's coordinates


	if (nodes.size() == 0) return;

	vector<IntPair> tmp;
	for (size_t i = 0; i < tour.size(); i++) {
		tmp.push_back(nodes.at(tour.at(i)));
	}
	nodes.clear();
	for (size_t i = 0; i < tmp.size(); i++) {
		nodes.push_back(tmp.at(i));
	}
}

// Description: Calculates matrix of distances and matrix of paths between lambdas
void TSPSolver::SetMatrixes()
{
	int size = nodes.size();
	int maxPath = (int) mine->GetWidth()*mine->GetHeight()/sqrt(2.0); // this is the maximum possible path on this map
	int dist;
	vector<IntPair> pathToNode;

	for (int i = 0; i < size - 1; i++) {
		// Get start node coordinates
		IntPair firNode = nodes[i];
		int startX = firNode.first;
		int startY = firNode.second;

		for (int j = i + 1; j < size; j++) {
			dist = 0;
			pathToNode.clear();

			// Get target node coordinates
			IntPair secNode = nodes[j];
			int targetX = secNode.first;
			int targetY = secNode.second;

			// Map the distance to the IntPair of nodes between lambda node and lift node
			// Using method allows to transform the closed graph to unclosed graph
			// and still using closed graph's algorithms.
			//
			// NOTE: we need to calculate optimal path from the last lambda to the lift using Astar algorithm later.

			if (j == size - 1) {
				if (i == 0) {
					dist = 2*maxPath;		// distance between robot and lift
					// Map the distance to the IntPair of nodes
					distMatrix[IntPair (i, j)] = dist;
					continue;
				} else dist = 3*maxPath;	// distance between lambda and lift
			}

			// Get distance between nodes
			pathToNode = FindPath(startX, startY, targetX, targetY);
			if (pathToNode.empty()) {
				int pathlen = pathToNode.size();
				dist += pathlen;
			} else dist += mine->GetHeight()*mine->GetWidth();													// TBD: in this case it seems better to delete lambda from list and ignore it

			// Map the distance to the IntPair of nodes
			distMatrix[IntPair (i, j)] = dist;
			// Map the path to the IntPair of nodes
			pathMatrix[IntPair (i, j)] = pathToNode;
		}
	}
}

// Description: Returns distance between two nodes
int TSPSolver::GetDistance(const int & node1, const int & node2)
{
	vector<IntPair> path;
	int start, target;

	// Check node's order
	if (node1 < node2) {
		start = node1;
		target = node2;
	} else {
		start = node2;
		target = node1;
	}

	//path = pathMatrix[IntPair<int,int> (start, target)];

	//if (path.empty()) {
	//	int startX = nodes.at(start).first;
	//	int startY = nodes.at(start).second;
	//	int targetX = nodes.at(target).first;
	//	int targetY = nodes.at(target).second;
	//	path = FindPath(startX, startY, targetX, targetY);

	//	// Map the path to the IntPair of nodes
	//	pathMatrix[IntPair (start, target)] = path;
	//}
	//if (path.size() == 1 && path.back().first == -1 && path.back().second == -1) {
	//	return -1;
	//}

	//return path.size();


	int startX = nodes.at(start).first;
	int startY = nodes.at(start).second;
	int targetX = nodes.at(target).first;
	int targetY = nodes.at(target).second;
	return (abs(startX - targetX) + abs(startY - targetY));
}

// Description: Stores tour distance
void TSPSolver::SetTourDistance(int dist)
{
	this->tourDistance = dist;
}

// Description: Calculates tour distance
int TSPSolver::CalcTourDistance()
{
	int dist = 0;
	int size = tour.size();

	for (int i = 0; i < size - 1; i++)
		dist += GetDistance(tour.at(i), tour.at(i + 1));	// accumulate all distances

	// And back to the beginning
	dist += GetDistance(tour.at(size - 1), tour.at(0));

	return dist;
}

// Description: Solve TSP problem with Nearest Neighbour algorithm
void TSPSolver::CreateNearestNeighbourTour()
{
	set<int> nodeSet;			// temporary unique set of nodes
	set<int>::iterator itr;

	int nodeNum = nodes.size();
	for (int i = 0; i < nodeNum; i++)
		nodeSet.insert(i);		// set contains nodes's serial numbers

	int node = 0;
	for (int i = 1; i <= nodeNum; i++) {
		tour.push_back(node);			// add to the tour a new node (which is the nearest to the previous one)
		itr = nodeSet.find(node);		// remove lambda from unique set
		nodeSet.erase(itr);
		node = GetNearestNeighbour(node, nodeSet);	// find the nearest node concerning the lambda node
	}
}

// Description: Returns the nearest node concerning the specified node
int TSPSolver::GetNearestNeighbour(const int & node, set<int> & nodeSet)
{
	// This algorithm is too simple to comment it

	int targetNode = 0;

	int minDistance = 3*mine->GetWidth()*mine->GetHeight();
	set<int>::iterator itr;
	for (itr = nodeSet.begin(); itr != nodeSet.end(); itr++) {
		int currNode = *itr;
		if (currNode == targetNode) continue;
		int dist = GetDistance(node, currNode);
		if (dist == -1) continue;
		else if (currNode == *(--nodeSet.end())) {
			int maxPath = (int) mine->GetWidth()*mine->GetHeight()/sqrt(2.0); // this is the maximum possible path on the map
			if (node == 0) dist = 2*maxPath;	// distance between robot and lift
			else dist += 3*maxPath;				// distance between lambda and lift
		}
		if (dist < minDistance) {
			targetNode = currNode;
			minDistance = dist;
		}
	}
	return targetNode;
}

// Description: Optimize TSP problem's solution with 2-opt algorithm
void TSPSolver::StartTwoOpt()
{
	int size = tour.size();

	for ( int i = 0; i < size - 3; i++ ) {
		for ( int j = i + 3; j < size - 1; j++ ) {
			TwoOpt(i, i + 1, j, j + 1);		// use 2-opt algorithm with selected nodes
		}
	}

	SetTourDistance(CalcTourDistance());	// recalculating tour distance
}

// Description: 2-opt algorithm
void TSPSolver::TwoOpt(const int & startN1Index, const int & targetN1Index,
					   const int & startN2Index, const int & targetN2Index)
{
	// Feasible exchanges only
	if (startN2Index == startN1Index || startN2Index == targetN1Index
		|| targetN2Index == startN1Index || targetN2Index == targetN1Index)
		return;

	// Remember original node's order
	int targetNode1old = tour.at(targetN1Index);
	int startNode2old = tour.at(startN2Index);
	// Remember original tour distance
	int old_dist = CalcTourDistance();	// calculating and remember old tour distance
	// Swap targetNode1 and startNode2 values
	int tmp = tour.at(targetN1Index);
	tour[targetN1Index] = tour.at(startN2Index);
	tour[startN2Index] = tmp;

	int new_dist = CalcTourDistance();	// calculating new tour distance
	// If there is no immprovement then cancel swaping
	if (new_dist >= old_dist) {
		tour.at(targetN1Index) = targetNode1old;
		tour.at(startN2Index) = startNode2old;
	}
}


// Description: Finds a path using A*.
vector<IntPair> TSPSolver::FindPath(int startX, int startY, int targetX, int targetY, bool useHcost)
{
	vector<IntPair> resultPath;			// vector of coordinates of cells in found path
	int path = 0;
	const int nonexistent = 0, found = 1;		// path-related constants
	const int inOpenList = 1, inClosedList = 2;	// lists-related constants
	int parentX, parentY, Gcost, index;
	int ** whichList;			// used to record whether a cell is on the open list or on the closed list.
	IntPair ** parent;	// used to record parent of each cage
	OpenListItem * openList;	// array holding open list items, which is maintained as a binary heap.
	int numberOfOpenListItems;

// 1. Checking start and target cells to avoid misunderstandings.

	// If start and target cells are the same cell
	if (startX == targetX && startY == targetY) {
		resultPath.push_back(IntPair (startX, startY));
		return resultPath;
	}

	// If target cell is unwalkable
	if (mine->GetMap()[targetX][targetY] == '#') {
		resultPath.push_back(IntPair (-1, -1));	// its better than return an empty vector
		return resultPath;
	}

// 2. Allocate memory and reset variables

	whichList = new int* [mine->GetHeight() + 1];
	for (int i = 0; i < mine->GetHeight(); i++) {
		whichList[i] = new int [mine->GetWidth() + 1];
		for (int j = 0; j < mine->GetWidth(); j++)
			whichList[i][j] = 0;
	}

	parent = new IntPair* [mine->GetHeight() + 1];
	for (int i = 0; i < mine->GetHeight(); i++) {
		parent[i] = new IntPair [mine->GetWidth() + 1];
	}

	openList = new OpenListItem [mine->GetWidth()*mine->GetHeight()+2];

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
					if (x != -1 && y != -1 && x != mine->GetHeight() && y != mine->GetWidth()) {
						
						// If not already on the closed list (items on the closed list have already been considered and can now be ignored).
						if (whichList[x][y] != inClosedList) {
							// If not a wall/obstacle square.
							if (mine->GetMap()[x][y] != '#' && mine->GetMap()[x][y] != '*') {																// TBD: use isWalkable(...) method from Field class
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
			resultPath.push_back(IntPair (x, y));

			if (x == startX && y == startY) break;

			// Look up the parent of the current cell
			tmp = parent[x][y].first;
			y = parent[x][y].second;
			x = tmp;
		} 

// 6.2. Reversing path

		int size = (int) resultPath.size();
		int amount = size/2;
		IntPair tmpIntPair;
		for (int i = 0; i < amount; i++) {
			tmpIntPair = resultPath[size - i - 1];
			resultPath[size - i - 1] = resultPath[i];
			resultPath[i] = tmpIntPair;
		}
		
	} else resultPath.push_back(IntPair (-1, -1));	// its better than return an empty vector

// 7. Freeing used memory

	for (int i = 0; i < mine->GetHeight(); i++)
		delete [] whichList[i];
	for (int i = 0; i < mine->GetHeight(); i++)
		delete [] parent[i];
	delete [] openList;

	return resultPath;
}


// Description: Deletes the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
void TSPSolver::DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength)
{
	heapLength--;								// decrease number of items in heap
	heap[1] = heap[heapLength + 1];				// move last item up to slot #1
	SinkItemInBinaryHeap(heap, heapLength, 1);	// move item to the properly position
	
}

// Description: Finds the item in the binary heap by cell's coordinates and returns its index.
int TSPSolver::GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y)
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
void TSPSolver::BubbleItemInBinaryHeap(OpenListItem * heap, int index)
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
void TSPSolver::SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index)
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

// Description: Builds result path as sequence of cells's coordinates
void TSPSolver::SetTourPath()
{
	for (int i = 0; i < (int) tour.size() - 1; i++) {
		// Peek two consecutive nodes
		int start = tour.at(i);
		int target = tour.at(i + 1);

		// Get path between this nodes from path matrix
		vector<IntPair> currpath = GetPath(start, target);

		// Path includes start node cell and target node cell also
		// So, we can simply ignore currpath[0]
		for (int j = 1; j < (int) currpath.size(); j++) {
			path.push_back(currpath.at(j));
		}
	}
}
