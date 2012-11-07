UTconvert: Field.cpp Field.h Game.cpp Game.h OpenListItem.cpp OpenListItem.h GUI-ascii.cpp GUI-ascii.h main.cpp  TSPSolver.cpp TSPSolver.h stdafx.cpp stdafx.h
	g++ -o GUI Simulator.cpp Field.cpp Game.cpp OpenListItem.cpp GUI-ascii.cpp main.cpp  TSPSolver.cpp stdafx.cpp -lncurses
