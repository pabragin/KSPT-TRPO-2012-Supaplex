#include "GameHistory.h"


GameHistory::GameHistory()
{
	undoStack.clear();
	redoStack.clear();
}

GameHistory::~GameHistory(void)
{
}

void GameHistory::Reset()
{
	undoStack.clear();
	redoStack.clear();
}

// Description: Saves current game state
void GameHistory::SaveState(Game game)
{
	undoStack.push_back(game);
	redoStack.clear();
}

int GameHistory::Undo(Game game)
{
	if (undoStack.empty()) return -1;
	state = undoStack.back();
	undoStack.pop_back();
	redoStack.push_back(game);
	return 0;
}

int GameHistory::Redo()
{
	if (redoStack.empty()) return -1;
	state = redoStack.back();
	redoStack.pop_back();
	undoStack.push_back(state);
	return 0;
}

Game GameHistory::GetGameState()
{
	return this->state;
}
