// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "OpenListItem.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>

using namespace std;

typedef pair<int, int> IntPair;

typedef char _MineObject;
typedef char _Command;

#define ROBOT 'R'
#define STONE '*'
#define WALL '#'
#define EARTH '.'
#define LAMBDA '\\'
#define CLOSED_LIFT 'L'
#define OPENED_LIFT 'O'
#define EMPTY ' '

#define RIGHT 'R'
#define LEFT 'L'
#define UP 'U'
#define DOWN 'D'
#define WAIT 'W'
#define ABORT 'A'

// TODO: reference additional headers your program requires here
