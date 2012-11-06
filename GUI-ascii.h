#include <curses.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <cstring>
=======
>>>>>>> origin/master
#include <string>
#include <stdio.h>

using namespace std;

class GUI
{
	public:
		GUI();
<<<<<<< HEAD
		~GUI();
		int current_window;//0-nothing, 1-game_win, 2-help_hame_win, 3-about_game_win, 4-game_win with commands window
=======
		int current_window;//0-nothing, 1-game_win, 2-help_hame_win, 3-about_game_win
>>>>>>> origin/master
		void draw_map(char **map, int column, int row, WINDOW *game_win);//draw map
		void draw_points(int Score, int Moves, int Lambdas, const char *Mov, WINDOW **frames);//draw current Score, Moves, Lambdas, 
		void resize_refresh();//refresh game field
	private:
		WINDOW *game_win;
		WINDOW *gamename;//game title
		WINDOW *menubar;//line in the top with
		WINDOW *commands_line;//commands line must be input here
		int x;//terminal size
		int y;
<<<<<<< HEAD
		string str;//string of commands
		string strC;//string of commands in command window, for resizing
=======
>>>>>>> origin/master
		void init_curses();
		void draw_menubar();
		WINDOW **draw_game_win();
		void about_game_win();
		void help_game_win();
		void delete_menu(WINDOW **items,int count);
		WINDOW **draw_menu();
		WINDOW **draw_menu_help();
		int scroll_menu(WINDOW **items);
		int scroll_help(WINDOW **items);
		void draw_game_name();
		bool hotkeys(int key);
		WINDOW **draw_enter_commands();
		int enter_commands_line(WINDOW **items);
<<<<<<< HEAD
		int input_Line();
=======
		string input_Line();
>>>>>>> origin/master
}; 












