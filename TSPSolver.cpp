#include "TSPSolver.h"
#include "Astar.h"


TSPSolver::TSPSolver(Field & amine)
{
	this->mine = amine;

	if (!mine.GetLambdas().empty()) {
		nodes.push_back(mine.GetRobot());
		for (int i = 0; i < (int) mine.GetLambdas().size(); i++) {
			nodes.push_back(mine.GetLambdas().at(i));
		}
		nodes.push_back(mine.GetLift());
	}
}

TSPSolver::~TSPSolver(void)
{
}

vector<int> TSPSolver::GetTour()
{
	return this->tour;
}

int TSPSolver::GetTourDistance()
{
	return this->tourDistance;
}

void TSPSolver::Solve()
{
	SetDistanceMatrix();
	CreateNearestNeighbourTour();
    StartTwoOpt();
}

vector<pair<int, int>> TSPSolver::GetNodes()
{
	return this->nodes;
}

int TSPSolver::GetNodeFromTour(int index)
{
	return tour[index];
}

// Calculate matrix of distances between lambdas
void TSPSolver::SetDistanceMatrix()
{
	Astar algAstar(mine.GetMap(), mine.GetWidth(), mine.GetHeight());

	int size = nodes.size();
	int maxPath = mine.GetWidth()*mine.GetHeight()/sqrt(2.0); // this is the maximum possible path on this map
	int dist;

	for (int i = 0; i < size - 1; i++) {
		// Get start node coordinates
		pair<int, int> firNode = nodes[i];
		int startX = firNode.first;
		int startY = firNode.second;

		for (int j = i + 1; j < size; j++) {
			dist = 0;

			// Get target node coordinates
			pair<int, int> secNode = nodes[j];
			int targetX = secNode.first;
			int targetY = secNode.second;

			// Map the distance to the pair of nodes between lambda node and lift node
			// Using method allows to transform the closed graph to unclosed graph
			// and still using closed graph's algorithms.
			//
			// NOTE: we need to calculate optimal path from the last lambda to the lift using Astar algorithm later.

			if (j == size - 1) {
				if (i == 0) {
					dist = 2*maxPath;		// distance between robot and lift
					// Map the distance to the pair of nodes
					distMatrix[pair<int, int> (i, j)] = dist;
					continue;
				} else dist = 3*maxPath;	// distance between lambda and lift
			}

			// Get distance between nodes
			int result = algAstar.FindPath(startX, startY, targetX, targetY, true);
			if (result == 1) dist += algAstar.GetPathLength();									// TBD: wow, this should be saved!
			else dist += mine.GetHeight()*mine.GetWidth();										// TBD: in this case it seems better to delete lambda from list and ignore it

			// Map the distance to the pair of nodes
			distMatrix[pair<int, int> (i, j)] = dist;
		}
	}
}

// Get distance between two nodes
int TSPSolver::GetDistance(const int & start, const int & target)
{
	// Check node's order
	if (start < target)
		return distMatrix[pair<int,int> (start, target)];
	else
		return distMatrix[pair<int,int> (target, start)];
}

// Store tour distance
void TSPSolver::SetTourDistance(int dist)
{
	this->tourDistance = dist;
}

// Calculate tour distance
int TSPSolver::CalcTourDistance()
{
	int dist = 0;
	int size = tour.size();

	for (int i = 0; i < size - 1; i++)
		dist += GetDistance(tour.at(i), tour.at(i + 1));	// accumulate all distances

	// And back to the beginning																// Remove it?!
	dist += GetDistance(tour.at(size - 1), tour.at(0));

	return dist;
}

// Solve TSP problem with Nearest Neighbour algorithm
void TSPSolver::CreateNearestNeighbourTour()
{
	set<int> nodeSet;			// temporary unique set of nodes
	set<int>::iterator itr;

	int nodeNum = nodes.size();
	for (int i = 0; i < nodeNum; i++)
		nodeSet.insert(i);	// set contains nodes's serial numbers

	int node = 0;
	for (int i = 1; i <= nodeNum; i++) {
		tour.push_back(node);			// add to the tour a new node (which is the nearest to the previous one)
		itr = nodeSet.find(node);		// remove lambda from unique set
		nodeSet.erase(itr);
		node = GetNearestNeighbour(node, nodeSet);	// find the nearest node concerning the lambda node
	}
}

// Returns the nearest node concerning the specified node
int TSPSolver::GetNearestNeighbour(const int & node, set<int> & nodeSet)
{
	int targetNode = 0;

	int minDistance = 3*mine.GetWidth()*mine.GetHeight();
	set<int>::iterator itr;
	for (itr = nodeSet.begin(); itr != nodeSet.end(); itr++) {
		int currNode = *itr;
		if (currNode == targetNode) continue;
		int dist = GetDistance(node, currNode);
		if (dist < minDistance) {
			targetNode = currNode;
			minDistance = dist;
		}
	}
	return targetNode;
}

// Optimize TSP problem's solution with 2-opt algorithm
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

// 2-opt algorithm
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
	if (new_dist > old_dist) {
		tour.at(targetN1Index) = targetNode1old;
		tour.at(startN2Index) = startNode2old;
	}
}