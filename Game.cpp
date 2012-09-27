#include "Game.h"


Game::Game(void)
{
	score = 0;
}


Game::~Game(void)
{
}

int Game::Start(char * file)
{
	int result = map.LoadMap(file);
	if (result != 0) {
		cout << "Unknown file." << endl;
		return -1;
	}

	robot.init(&map.GetRobot(), &map.GetLambdas());

//	map.printMap(cout);

	return 0;
}

void Game::Solve()
{
	if (!map.GetMap()) return;
	while (true) {
		robot.Move(map.GetMap());
		map.UpdateMap();
	}
}
