#pragma once

#include "Game.h"

class GameHistory
{
	Game state;
	vector<Game> undoStack;
	vector<Game> redoStack;
public:
	GameHistory();
	~GameHistory(void);

	void Reset();
	void SaveState(Game game);

	int Undo(Game game);
	int Redo();

	Game GetGameState();
};
