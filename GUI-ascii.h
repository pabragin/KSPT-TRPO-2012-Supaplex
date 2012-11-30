#include <curses.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Game.h"
#include "FileManager.h"
#include "stdafx.h"
#include <pthread.h>

using namespace std;

class GUI
{
	public:
		GUI();
		~GUI();
		Game game;
		int NewGame(const char *FileName);
		int current_window;//0-nothing, 1-game_win, 2-help_hame_win, 3-about_game_win, 4-game_win with commands window
		//5-list of files, 6-move delay;
		void draw_map(char **map, int column, int row, int start_x, int start_y, WINDOW *game_win);//draw map
		void draw_points(int Score, int Moves, int Lambdas, const char *Mov, WINDOW **frames);//draw current Score, Moves, Lambdas, 
		void resize_refresh();//refresh game field
	private:
		WINDOW *game_win;
		WINDOW *gamename;//game title
		WINDOW *menubar;//line in the top with
		WINDOW *commands_line;//commands line must be input here
		WINDOW *time_line;//time line must be input here
		int x;//terminal size
		int y;
		FileManager *fm;//File manager
		string str;//string of commands
		string strC;//string of commands in command window, for resizing
		string strT;//string of Time in time window, for resizig
		string CurrentPath;//В каком каталоге находимся
		int selectedMap;
		bool firstNewGame;
        bool firstStart;
        void init_curses();
		void draw_menubar();
		WINDOW **draw_game_win();
		void about_game_win();
		void help_game_win();
		void delete_menu(WINDOW **items,int count);
		WINDOW **draw_menu();
		WINDOW **draw_menu_help();
		WINDOW **maps_win();
		int scroll_menu(WINDOW **items);
		int scroll_help(WINDOW **items);
		int scroll_maps(WINDOW **items);
		void draw_game_name();
		void hotkeys(int key);
		WINDOW **draw_enter_commands();
		WINDOW **draw_time_line();
		int input_Line();
		void start(istream & sin);
		int startx, starty; //начальное положение карты
		void solve_map(void);//функция запуска решателя
		void RobotCentred(void);//Выставление начальных координат на робота
		int time_Line(void);//функция ввода времени
		void DrawAddForMenu(void);//дополнительное меню в шапке
}; 












