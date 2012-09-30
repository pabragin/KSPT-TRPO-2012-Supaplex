#pragma once

#include "stdafx.h"
#include "Field.h"
#include <map>
#include <set>

class TSPSolver
{
	Field * mine;
	map<pair<int, int>, int> distMatrix;
	map<pair<int, int>, vector<pair<int, int>>> pathMatrix;

	vector<pair<int, int>> path;
	vector<pair<int, int>> nodes;
	vector<int> tour;
	int tourDistance;

	vector<Field> snapshot;
public:
	TSPSolver(Field * amine);
	~TSPSolver(void);
	
	vector<pair<int, int>> GetTourPath();
	vector<pair<int, int>> GetPath(const int & start, const int & target);
	vector<pair<int, int>> GetNodes();
	vector<int> GetTour();
	int GetTourDistance();

	void Solve();

private:
	void SetMatrixes();
	int GetDistance(const int & start, const int & target);
	void SetTourDistance(int dist);
	int CalcTourDistance();

	void MakeSnapshot();
	void LoadSnapshot();

	void CreateNearestNeighbourTour();
	int GetNearestNeighbour(const int & node, set<int> & nodeSet);

	void StartTwoOpt();
	void TwoOpt(const int & startN1Index, const int & targetN1Index,
				const int & startN2Index, const int & targetN2Index);

	void SetTourPath();
};

