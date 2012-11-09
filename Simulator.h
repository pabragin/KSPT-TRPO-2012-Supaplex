#pragma once

#include "Field.h"
#include <map>

class Simulator
{
	Field mine;

	vector<Field> snapshot;
	bool robotIsDead;

	vector<IntPair> path;
	vector<IntPair> missedLambdas;
	vector<int> unexpectedLambdas;
public:
	Simulator(Field & amine);
	~Simulator(void);

	vector<IntPair> GetPath();

	void StartSimulation(vector<IntPair> waypoints);

    bool IsLiftBlocked();
    
private:
	void UpdateMap();	// updates map according to the rules

	int MoveRobotToTarget(IntPair target);
	int Step(IntPair cell, int Gcost, OpenListItem * openList, int & numberOfOpenListItems, int ** whichList, 
		IntPair ** parent, IntPair target, OpenListItem * closedList, int & numberOfClosedListItems);

	bool IsDeadLock(int x, int y);

	int FindMissedLambda(IntPair lambda);
	bool FindUnexpectedLambda(int index);

	bool MoveRobot(int x, int y);

	void MakeSnapshot();
	void LoadSnapshot();

	void AddAdjacentCellsToOpenList(OpenListItem * openList, int & numberOfOpenListItems, int parentX, 
		int parentY, int Gcost, int ** whichList, IntPair ** parent, IntPair target, 
		OpenListItem * closedList, int & numberOfClosedListItems);

	void DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength);
	int GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y);
	void BubbleItemInBinaryHeap(OpenListItem * heap, int index);
	void SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index);
};
