#pragma once

#include "Field.h"
#include <map>

class Simulator
{
	Field mine;

	vector<Field> snapshot;
	bool robotIsDead;

	vector<pair<int, int>> path;
	vector<pair<int, int>> missedLambdas;
	vector<int> unexpectedLambdas;
public:
	Simulator(Field & amine);
	~Simulator(void);

	vector<pair<int, int>> GetPath();

	void StartSimulation(vector<pair<int, int>> waypoints);

private:
	void UpdateMap();	// updates map according to the rules

	int MoveRobot(pair<int, int> target);

	int FindMissedLambda(pair<int, int> lambda);
	bool FindUnexpectedLambda(int index);

	void Step(int x, int y);
	void MakeSnapshot();
	void LoadSnapshot();

	void AddAdjacentCellsToOpenList(OpenListItem * openList, int & numberOfOpenListItems, int parentX, 
		int parentY, int Gcost, int ** whichList, pair<int, int> ** parent, pair<int, int> target, 
		OpenListItem * closedList, int & numberOfClosedListItems);

	void DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength);
	int GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y);
	void BubbleItemInBinaryHeap(OpenListItem * heap, int index);
	void SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index);
};