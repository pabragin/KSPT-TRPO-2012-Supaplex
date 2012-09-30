#pragma once

#include "stdafx.h"

class Astar																												// TBD: move it into TSPSolver class?
{
	char ** map;
	int mapWidth;
	int mapHeight;
	
	vector<pair<int, int>> resultPath;

public:
	Astar(void);
	Astar(char **, int m, int n);
	~Astar(void);

	vector<pair<int, int>> GetResultPath();
	int GetPathLength();

	int FindPath(int startX, int startY, int targetX, int targetY, bool useHcost = true);	// Finds a path using A*.	// TBD: using useHcost = false (i.e. Dijkstra instead of Astar) is useless?

private:
	void DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength);
	int GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y);
	void BubbleItemInBinaryHeap(OpenListItem * heap, int index);
	void SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index);
};

