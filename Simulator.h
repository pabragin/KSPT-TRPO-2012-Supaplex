#pragma once

#include "Field.h"

class Simulator
{
	Field * mine;

	vector<Field> snapshot;
	bool robotIsDead;

	vector<pair<int, int>> path;
public:
	Simulator(Field * amine);
	~Simulator(void);

	vector<pair<int, int>> GetPath();

	void StartSimulation(vector<pair<int, int>> waypoints);

private:
	void UpdateMap();	// updates map according to the rules

	int MoveRobot(pair<int, int> target);

	void Step(int x, int y);
	void MakeSnapshot();
	void LoadSnapshot();


	void DeleteTopItemFromBinaryHeap(OpenListItem * heap, int & heapLength);
	int GetItemIndexFromBinaryHeapByCoord(OpenListItem * heap, int heapLength, const int & x, const int & y);
	void BubbleItemInBinaryHeap(OpenListItem * heap, int index);
	void SinkItemInBinaryHeap(OpenListItem * heap, int heapLength, int index);
};

