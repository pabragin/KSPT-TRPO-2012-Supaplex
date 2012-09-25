#pragma once

#include "stdafx.h"

class Astar
{
	char ** map;
	int mapWidth;
	int mapHeight;
	int numberOfOpenListItems;
	OpenListItem * openList; // array holding open list items, which is maintained as a binary heap.
	Cell * resultPath;
	int pathLength;

public:
	Astar(void);
	Astar(char **, int m, int n);
	~Astar(void);

	Cell * GetResultPath();
	int GetPathLength();

	void InitOpenList(void);	// Allocates memory for the open list.
	void FreeOpenList(void);	// Frees memory used by the open list.
	int FindPath(int, int, int, int);	// Finds a path using A*.

private:
	void DeleteTopItemFromBinaryHeap();
	void AddItemToBinaryHeap();
	int GetItemIndexFromBinaryHeapByCoord(int, int);
	void BubbleItemInBinaryHeap(int);
};

