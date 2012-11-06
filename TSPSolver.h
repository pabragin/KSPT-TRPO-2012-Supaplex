#pragma once

#include "stdafx.h"
#include "Field.h"
#include <map>
#include <set>

class TSPSolver
{
	Field * mine;
	map<pair<int, int>, int> distMatrix;
	map<pair<int, int>, CoordinatesVector> pathMatrix;

	CoordinatesVector path;
	CoordinatesVector nodes;
	vector<int> tour;
	int tourDistance;
public:
	TSPSolver(Field * amine);
	~TSPSolver(void);
	
	CoordinatesVector GetTourPath();
	CoordinatesVector GetPath(const int & start, const int & target);
	CoordinatesVector GetNodes();
	vector<int> GetTour();
	int GetTourDistance();

	void Solve(const int & iterations);

private:
	void SetMatrixes();
	int GetDistance(const int & node1, const int & node2);
	void SetTourDistance(int dist);
	int CalcTourDistance();

	void CreateNearestNeighbourTour();
	int GetNearestNeighbour(const int & node, set<int> & nodeSet);

	void StartTwoOpt();
	void TwoOpt(const int & startN1Index, const int & targetN1Index,
				const int & startN2Index, const int & targetN2Index);

	vector<IntPair> FindPath(int startX, int startY,
									int targetX, int targetY,
									bool useHcost = true);	// Finds a path using A*.			// TBD: using useHcost = false (i.e. Dijkstra instead of Astar) is useless?
	void DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength);
	int GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y);
	void BubbleItemInBinaryHeap(OpenListItem * heap, int index);
	void SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index);

	void SetTourPath();
};
