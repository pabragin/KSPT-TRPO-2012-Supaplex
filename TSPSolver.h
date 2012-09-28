#pragma once

#include "stdafx.h"
#include "Field.h"
#include <map>
#include <set>

class TSPSolver
{
	Field mine;
	map<pair<int, int>, int> distMatrix;

	vector<pair<int, int>> nodes;
	vector<int> tour;
	int tourDistance;
public:
	TSPSolver(Field & map);
	~TSPSolver(void);
	
	vector<int> GetTour();
	vector<pair<int, int>> GetNodes();
	int GetTourDistance();

	void Solve();

private:
	int GetNodeFromTour(int index);

	void SetDistanceMatrix();
	int GetDistance(const int & lambda1, const int & lambda2);
	void SetTourDistance(int dist);
	int CalcTourDistance();

	void CreateNearestNeighbourTour();
	int GetNearestNeighbour(const int & lambda, set<int> & lambdaSet);

	void StartTwoOpt();
	void TwoOpt(const int & startLambda1, const int & targetLambda1,
				const int & startLambda2, const int & targetLambda2);
};

