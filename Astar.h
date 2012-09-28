#pragma once

#include "stdafx.h"

class Astar
{
	char ** map;
	int mapWidth;
	int mapHeight;
	int numberOfOpenListItems;
	OpenListItem * openList;	// array holding open list items, which is maintained as a binary heap.
	vector<pair<int, int>> resultPath;

public:
	Astar(void);
	Astar(char **, int m, int n);
	~Astar(void);

	vector<pair<int, int>> GetResultPath();
	int GetPathLength();

	int FindPath(int startX, int startY, int targetX, int targetY, bool useHcost);	// Finds a path using A*.

private:
	void InitOpenList(void);	// Allocates memory for the open list.
	void FreeOpenList(void);	// Frees memory used by the open list.
	void DeleteTopItemFromBinaryHeap();
	void AddItemToBinaryHeap();
	int GetItemIndexFromBinaryHeapByCoord(int, int);
	void BubbleItemInBinaryHeap(int);
};

