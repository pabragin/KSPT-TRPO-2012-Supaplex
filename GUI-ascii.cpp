#include "GUI-ascii.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>


#define ENTER 10
#define ESCAPE 27
#define BACKSPACE 127

GUI::GUI(){
	int key, selected_from_m, selected_from_h;
	startxy= make_pair(0, 0);
	init_curses();
	ESCDELAY=0;
	termxy= make_pair(getmaxx(stdscr), getmaxy(stdscr));//terminal size
	game_win=subwin(stdscr,termxy.second-5,termxy.first-4,4,2);//size of game window
	wbkgd(game_win,COLOR_PAIR(2));

	draw_game_name();
	draw_menubar();
	selectedMap=-1;
	current_window=0;//current window is empty
	do
	{
        WINDOW **menu_items, **help_items;
		key=getch();
	if (key==KEY_F(1)) {	
        menu_items=draw_menu();
		selected_from_m=scroll_menu(menu_items);
		delete_menu(menu_items, 6);
		if (selected_from_m==0)
		{
			current_window=5;
			resize_refresh();
		}
		else if(selected_from_m==1)
		{
			if(selectedMap!=-1)
			{
				if(NewGame(Files[selectedMap].c_str())!=-1)
					current_window=1;
				else
					current_window=0;
				resize_refresh();
			}
		}
		else if(selected_from_m==3)
		{
			solve_map();
			resize_refresh();
		}
	}
	else if (key==KEY_F(2)) 
	{
        help_items=draw_menu_help();
        selected_from_h=scroll_help(help_items);
		delete_menu(help_items, 3);
		if (selected_from_h==0)
		{
			current_window=2;
			resize_refresh();	
		}
		else if (selected_from_h==1)
		{
			current_window=3;
			resize_refresh();
		}
	}
	hotkeys(key);
	//printw("%i",key);//print number of char
	if (key==KEY_RESIZE){
	  resize_refresh();
	}
	}while (selected_from_m!=4&&key!=5);//exit from game if select exit from menu or ctrl-e pressed
    	endwin();
}

GUI::~GUI()
{
}

void GUI::start(istream & sin) {
	if (game.Init(sin) == -1) 
	{
		printf("Initialization Fail");
	}
}

int GUI::NewGame(const char *FileName)
{
	str="";
	strC="";
	startxy=make_pair(0,0);
	ifstream fin;
	fin.open(FileName);
	if (!fin.is_open()) 
	{
		return -1;
	}
	start(fin);
	return 0;
}

void GUI::solve_map(void)
{
	if(selectedMap!=-1)
		{
			if(NewGame(Files[selectedMap].c_str())!=-1)
			{
				game.Solve(1);
				for(unsigned int i=0; i<game.GetTrace().size(); i++)
				{
					//y-10,x-20,4,2)
					if(termxy.first-22>game.GetField()->GetWidth()-1)
					{
						startxy.first=(termxy.first-22)/2;
					}
					game.MoveRobot(game.GetTrace()[i]);
					str+=game.GetTrace()[i];
					resize_refresh();
					usleep(40000);
				}
				current_window=1;
			}
			else
				current_window=0;
		}
}

void GUI::hotkeys(int key){
	switch (key){
		case 119://"w" up
		if(current_window==1 && game.GetResult()==0)
		{
			game.MoveRobot(UP);
			str+="U";
			resize_refresh();
		}
		break;
		case 115://"s" down
		if(current_window==1 && game.GetResult()==0)
		{
			game.MoveRobot(DOWN);
			str+="D";
			resize_refresh();
		}
		break;
		case 97://"a" left button
		if(current_window==1 && game.GetResult()==0)
		{
			game.MoveRobot(LEFT);
			str+="L";
			resize_refresh();
		}
		break;
		case 100://"d"right button
		if(current_window==1 && game.GetResult()==0)
		{
			game.MoveRobot(RIGHT);
			str+="R";
			resize_refresh();
		}
		break;
		case 113://"q" abort button
		if(current_window==1 && game.GetResult()==0)
		{
			game.MoveRobot(ABORT);
			str+="A";
			resize_refresh();
		}
		break;
	   case 101://"e"wait button
		if(current_window==1 && game.GetResult()==0)
		{
			game.GetField()->UpdateMap();
			str+="W";
			resize_refresh();
		}
		break;
	   case 259://up button, scroll up
		if(current_window==1&& startxy.second!=0)
		{
			startxy.second--;
			resize_refresh();
		}
		break;
	  case 258://down button, scroll down
		if(current_window==1)
		{
			startxy.second++;
			resize_refresh();
		}
		break;
	  case 260://left button, scroll left
		if(current_window==1&& startxy.first!=0)
		{
			startxy.first--;
			resize_refresh();
		}
		break;
	  case 261://right button, scroll right
		if(current_window==1)
		{
			startxy.first++;
			resize_refresh();
		}
		break;
	  case 14://ctrl-n
	    current_window=5;
	    resize_refresh();
	    break;
	  case 18://ctrl-r
	    if(selectedMap!=-1)
	    {
			if(NewGame(Files[selectedMap].c_str())!=-1)
				current_window=1;
				else
				current_window=0;
		}
		resize_refresh();
	    break;
	  case 15://ctrl-o
		solve_map();
		resize_refresh();
	    break;
	  case 263://ctrl-h
	    current_window=2;
	    resize_refresh();
	    break;
	  case 1://ctrl-a
	    current_window=3;
	    resize_refresh();
	    break;
	  case 11://ctrl-k
	  if(current_window==1){
	    current_window=4;
	    resize_refresh();
	    break;
	  }
	  else 
	    break;
	  default:
	    break;
	}   
}
int GUI::input_Line(){
	int key;
	int xx;
	int yy;
	if(strC.size()!=0){
		if (strC.size()<(unsigned)(getmaxx(commands_line)*(getmaxy(commands_line)))-1)
		{
		    waddstr(commands_line,strC.c_str());
		    wrefresh(commands_line);
		}
		else
		{
		    waddstr(commands_line, strC.substr(strC.size()-(getmaxx(commands_line)*(getmaxy(commands_line)))+1, strC.size()).c_str());
		    wrefresh(commands_line);
		}
	}
	bool f=true;
	while(f==true){
	key=getch();
	switch (key)
	{
	  case 'u': waddstr(commands_line,"U");
		    wrefresh(commands_line);
		    strC+="U";
		    break;
	  case 'd': waddstr(commands_line,"D");
		    wrefresh(commands_line);
		    strC+="D";
		    break;
	  case 'l': waddstr(commands_line,"L");
		    wrefresh(commands_line);
		    strC+="L";
		    break;
	  case 'r': waddstr(commands_line,"R");
		    wrefresh(commands_line);
		    strC+="R";
		    break;
	  case 'w': waddstr(commands_line,"W");
		    wrefresh(commands_line);
		    strC+="W";
		    break;
	  case 'a': waddstr(commands_line,"A");
		    wrefresh(commands_line);
		    strC+="A";
		    break;
	  case BACKSPACE : getyx(commands_line, yy, xx);
			if (yy!=0 && xx==0)
		    {
				yy=yy-1;
				xx=getmaxx(commands_line);
				mvwaddstr(commands_line, yy, xx-1, " ");
				strC=strC.substr(0,strC.size()-1);
				wmove(commands_line, yy, xx-1);
				wrefresh(commands_line);
		    }
		    else if (yy==0 && xx == 0)
		    {
				wmove(commands_line, 0, 0);
				werase(commands_line);
				if(strC.size()>=(unsigned)getmaxx(commands_line)*(getmaxy(commands_line)-1))
				{
					waddstr(commands_line, strC.substr(strC.size()-(getmaxx(commands_line)*(getmaxy(commands_line)-1)), strC.size()).c_str());
				}
				else
				{
					waddstr(commands_line, strC.c_str());
				}
				wrefresh(commands_line);
			}
		    else
		    {
		        mvwaddstr(commands_line, yy, xx-1, " ");
		        strC=strC.substr(0,strC.size()-1);
		        wmove(commands_line, yy, xx-1);
		        wrefresh(commands_line);
		    }
		    break;
		case ENTER :
			//str=strC;
			current_window=1;
			f=false;
			if(game.GetResult()==0)
			{
				for(unsigned int i=0; i<strC.size(); i++)
				{
					if(game.GetResult()==0)
					{
						game.MoveRobot(strC[i]);
						str+=strC[i];
						resize_refresh();
						usleep(40000);
					}
				}
			}
			return 0;
			break;
		case KEY_RESIZE :
			resize_refresh();
			return 0;
			break;
		case ESCAPE :
			current_window=1;
			f=false;
			resize_refresh();
			return 0;
			break;
	}
	getyx(commands_line, yy, xx);
	if (yy==getmaxy(commands_line)-1 && xx == getmaxx(commands_line)-1)
	{
		wmove(commands_line, 0, 0);
		werase(commands_line);
		waddstr(commands_line, strC.substr(strC.size()-(getmaxx(commands_line)*(getmaxy(commands_line)-1)), strC.size()).c_str());
		wrefresh(commands_line);
	}
	}
	return 0;
}
void GUI::resize_refresh(){
	termxy.first = getmaxx(stdscr);//terminal size
	termxy.second = getmaxy(stdscr);
	werase(stdscr);
	if(termxy.first>20 && termxy.second>10)
	{
		draw_game_name();
		draw_menubar();
		if (current_window==1)
		{
			WINDOW **gw = draw_game_win();
			string s= str;
			draw_points(game.GetScore(), game.GetMoves(), game.GetCollectedLambdasNum(), s.c_str(), gw);
			draw_map(game.GetField()->GetMap(), game.GetField()->GetWidth()-1, game.GetField()->GetHeight()-1, startxy.first, startxy.second, gw[0]);
			touchwin(stdscr);
			refresh();
		}
		else if (current_window==2){
			game_win=subwin(stdscr,termxy.second-5,termxy.first-4,4,2);//size of game window
			wbkgd(game_win,COLOR_PAIR(2));
			help_game_win();
			touchwin(stdscr);
			refresh();
		}
		else if (current_window==3)
		{
			game_win=subwin(stdscr,termxy.second-5,termxy.first-4,4,2);//size of game window
			wbkgd(game_win,COLOR_PAIR(2));
			about_game_win();
			touchwin(stdscr);
			refresh();
		}
		else if (current_window==4)
		{
			WINDOW **gw = draw_game_win();
			string s= str;
			draw_points(game.GetScore(), game.GetMoves(), 1, s.c_str(), gw);
			draw_map(game.GetField()->GetMap(), game.GetField()->GetWidth()-1, game.GetField()->GetHeight()-1, startxy.first, startxy.second, gw[0]);
			touchwin(stdscr);
			refresh();
			draw_enter_commands();
			input_Line();
		}
		else if (current_window==5)
		{
			GetListOfFiles();
			game_win=subwin(stdscr,termxy.second-5,termxy.first-4,4,2);//size of game window
			wbkgd(game_win,COLOR_PAIR(2));
			WINDOW **menu_items;
			menu_items=maps_win();
			selectedMap=scroll_maps(menu_items);
			if(selectedMap==-1)
			{
				current_window=0;
				return;
			}
			delete_menu(menu_items, 6);
			if(selectedMap!=-1)
			{
				if(NewGame(Files[selectedMap].c_str())!=-1)
					current_window=1;
				else
					current_window=0;
			}
			resize_refresh();
		}
		else if (current_window==0)
		{
			touchwin(stdscr);
			refresh();
		}
	}
	

}
void GUI::init_curses(){
    initscr();
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLUE);
    init_pair(2,COLOR_BLUE,COLOR_WHITE);
    init_pair(3,COLOR_RED,COLOR_WHITE);
	init_pair(4,COLOR_RED,COLOR_BLUE);
	init_pair(5,COLOR_BLUE,COLOR_BLUE);
	init_pair(6,COLOR_YELLOW,COLOR_WHITE);
	init_pair(7,COLOR_YELLOW,COLOR_BLUE);
	init_pair(8,COLOR_WHITE,COLOR_BLACK);
	init_pair(9,COLOR_BLACK,COLOR_WHITE);
	init_pair(10,COLOR_GREEN,COLOR_WHITE);
	init_pair(11,COLOR_MAGENTA,COLOR_WHITE);
	init_pair(12,COLOR_CYAN,COLOR_WHITE);
    curs_set(0);
    noecho();
    keypad(stdscr,TRUE);
}

void GUI::draw_menubar(){
	bkgd(COLOR_PAIR(1));
	menubar=subwin(stdscr,1,termxy.first,0,0);
	wbkgd(menubar,COLOR_PAIR(2));
	waddstr(menubar,"Menu");
	wattron(menubar,COLOR_PAIR(3));
	waddstr(menubar,"(F1)");
	wattroff(menubar,COLOR_PAIR(3));
	wmove(menubar,0,15);
	waddstr(menubar,"Help");
	wattron(menubar,COLOR_PAIR(3));
	waddstr(menubar,"(F2)");
	wattroff(menubar,COLOR_PAIR(3));
}

WINDOW **GUI::draw_game_win(){
	game_win=subwin(stdscr,termxy.second-5,termxy.first-4,4,2);//size of game window
	wbkgd(game_win,COLOR_PAIR(2));
	WINDOW **frames;
	frames=(WINDOW **)malloc(4*sizeof(WINDOW *));
	frames[0]=subwin(game_win,termxy.second-10,termxy.first-20,4,2);//игровое поле
	wbkgd(frames[0],COLOR_PAIR(2));
	box(frames[0],ACS_VLINE,ACS_HLINE);
	frames[1]=subwin(game_win,termxy.second-10,16,4,termxy.first-18);//очки
	wbkgd(frames[1],COLOR_PAIR(2));
	box(frames[1],ACS_VLINE,ACS_HLINE);
	frames[2]=subwin(game_win,3,termxy.first-4,termxy.second-6,2);//ходы
	wbkgd(frames[2],COLOR_PAIR(2));
	box(frames[2],ACS_VLINE,ACS_HLINE);
	frames[3]=subwin(game_win,3,termxy.first-4,termxy.second-4,2);//hotkeys
	wbkgd(frames[2],COLOR_PAIR(2));
	wmove(frames[1],1,1);
	wattron(frames[1],COLOR_PAIR(3));
    waddstr(frames[1],"Score:");
    wmove(frames[1],3,1);
    waddstr(frames[1],"Moves:");
	wmove(frames[1],5,1);
	waddstr(frames[1],"Lambdas:");
	wmove(frames[2],1,1);
    waddstr(frames[2],"Trace:");
    wmove(frames[3],1,1);
    waddstr(frames[3],"Movements: w, a, s, d; q-ABORT; e-WAIT; \"ctrl-k\" - command window");
    wmove(frames[3],2,1);
    waddstr(frames[3],"\"ctrl-o\" - solve; \"ctrl-n\" - new game;\"ctrl-r\" - reset.");
	touchwin(stdscr);
	refresh();
	return frames;
}

void GUI::draw_map(char **map, int column,int row, int start_x, int start_y, WINDOW *game_win)
{
	WINDOW *gameWin=subwin(game_win,termxy.second-12,termxy.first-22,5,3);
	wbkgd(gameWin,COLOR_PAIR(2));
	for(int i=start_y; i<=row; i++)
	{	
		for(int j=start_x; j<=column; j++)
		{
			switch (map[i][j]){
			case 'R':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(3));
				break;
			case '*':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(12));
				break;
			case '.':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(11));
				break;
			case '\\':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(6));
				break;
			case 'L':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(9));
				break;
			case 'O':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|COLOR_PAIR(9));
				break;
			case '@':
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]|A_BLINK);
				break;
			default:
				mvwinsch(gameWin, i-start_y, j-start_x, map[i][j]);
				break;
			}
		}
	}
	touchwin(stdscr);
    refresh();
}

void GUI::draw_points(int Score, int Moves, int Lambdas, const char *Mov, WINDOW **frames){
	char Score_str[5];
	sprintf(Score_str, "%d", Score);
	char Moves_str[5];
	sprintf(Moves_str, "%d", Moves);
	char Lambdas_str[5];
	sprintf(Lambdas_str, "%d", Lambdas);
	mvwaddstr(frames[1], 1, 8, Score_str);
	mvwaddstr(frames[1], 3, 8, Moves_str);
	mvwaddstr(frames[1], 5, 10, Lambdas_str);
	string s;
	s = Mov;
	if (s.size()>(unsigned)(termxy.first-13))
		mvwprintw(frames[2], 1,8, s.substr(s.size()-(termxy.first-13), s.size()).c_str());
	else
		mvwprintw(frames[2], 1,8, s.c_str());
	touchwin(stdscr);
    refresh();
}

void GUI::about_game_win()
{
	WINDOW *f=subwin(game_win,termxy.second-5,termxy.first-6,4,3);//игровое поле
	wbkgd(f,COLOR_PAIR(2));
	box(f,ACS_VLINE,ACS_HLINE);
	wmove(f,1,1);
	wattron(f,COLOR_PAIR(3));
    waddstr(f,"This game was created by:");
	wmove(f,2,1);
	waddstr(f,"Zaychenko Evgeny");
	wmove(f,3,1);
	waddstr(f,"Bragin Pavel");
	wmove(f,4,1);
	waddstr(f,"Hozyainov Ivan");
}

void GUI::help_game_win()
{
	WINDOW *f=subwin(game_win,termxy.second-5,termxy.first-6,4,3);//игровое поле
	wbkgd(f,COLOR_PAIR(2));
	box(f,ACS_VLINE,ACS_HLINE);
	wmove(f,1,1);
	wattron(f,COLOR_PAIR(3));
  	waddstr(f,"This game is not ready yet");
}

void GUI::delete_menu(WINDOW **items,int count)
{
    int i;
    for (i=0;i<count;i++)
    delwin(items[0]);
    free(items);
	touchwin(stdscr);
	refresh();
}

int GUI::scroll_menu(WINDOW **items){
        int key;
        int selected=0;
		int count = 5;
        while (1) {
                key=getch();
                if (key==KEY_DOWN || key==KEY_UP) {
                        wbkgd(items[selected+1],COLOR_PAIR(2));
			switch (selected){
			  case 0: wmove(items[1], 0,0);
				  wattron(items[1],COLOR_PAIR(6));
				  waddstr(items[1],"N");
				  wattroff(items[1],COLOR_PAIR(6));
			  case 1: wmove(items[2], 0,0);
				  wattron(items[2],COLOR_PAIR(6));
				  waddstr(items[2],"R");
				  wattroff(items[2],COLOR_PAIR(6));
				  break;
			  case 2: wmove(items[3], 0,0);
				  wattron(items[3],COLOR_PAIR(6));
				  waddstr(items[3],"M");
				  wattroff(items[3],COLOR_PAIR(6));
				  break;
			  case 3: wmove(items[4], 0,1);
				  wattron(items[4],COLOR_PAIR(6));
				  waddstr(items[4],"o");
				  wattroff(items[4],COLOR_PAIR(6));
				  break;
			  case 4: wmove(items[5], 0,0);
				  wattron(items[5],COLOR_PAIR(6));
				  waddstr(items[5],"E");
				  wattroff(items[5],COLOR_PAIR(6));
				  break;
				
			}
                        wnoutrefresh(items[selected+1]);
                        if (key==KEY_DOWN) {
                                selected=(selected+1) % count;
                        } else {
                                selected=(selected+count-1) % count;
                        }
                        wbkgd(items[selected+1],COLOR_PAIR(1));
			switch (selected){
			  case 0: wmove(items[1], 0,0);
				  wattron(items[1],COLOR_PAIR(7));
				  waddstr(items[1],"N");
				  wattroff(items[1],COLOR_PAIR(7));
			  case 1: wmove(items[2], 0,0);
				  wattron(items[2],COLOR_PAIR(7));
				  waddstr(items[2],"R");
				  wattroff(items[2],COLOR_PAIR(7));
				  break;
			  case 2: wmove(items[3], 0,0);
				  wattron(items[3],COLOR_PAIR(7));
				  waddstr(items[3],"M");
				  wattroff(items[3],COLOR_PAIR(7));
				  break;
			  case 3: wmove(items[4], 0,1);
				  wattron(items[4],COLOR_PAIR(7));
				  waddstr(items[4],"o");
				  wattroff(items[4],COLOR_PAIR(7));
				  break;
			  case 4: wmove(items[5], 0,0);
				  wattron(items[5],COLOR_PAIR(7));
				  waddstr(items[5],"E");
				  wattroff(items[5],COLOR_PAIR(7));
				  break;
			}
			wnoutrefresh(items[selected+1]);
                        doupdate();
                } else if (key==ESCAPE) {
                        return -1;
                } else if (key==ENTER) {
                        return selected;
                }
                if (key==KEY_RESIZE){
			resize_refresh();
			return -2;
		}
        }
}

int GUI::scroll_maps(WINDOW **items)
{
    int key;
    int selected=0;
    int count=Files.size();
    while (1) {
        key=getch();
        if (key==KEY_DOWN || key==KEY_UP) {
            wbkgd(items[selected+1],COLOR_PAIR(2));
            wnoutrefresh(items[selected+1]);
            if (key==KEY_DOWN) {
                selected=(selected+1) % count;
            } else {
                selected=(selected+count-1) % count;
            }
            wbkgd(items[selected+1],COLOR_PAIR(1));
            wnoutrefresh(items[selected+1]);
            doupdate();
        } else if (key==ESCAPE) {
            return -1;
        } else if (key==ENTER) {
            return selected;
        }
		else if (key==KEY_RESIZE) {
            resize_refresh();
            return -1;
        }
    }
}

int GUI::scroll_help(WINDOW **items){
        int key;
        int selected=0;
		int count =2;
        while (1) 
        {
                key=getch();
                if (key==KEY_DOWN || key==KEY_UP) 
                {
					wbkgd(items[selected+1],COLOR_PAIR(2));
					switch (selected)
					{
						case 0: wmove(items[1], 0,0);
						wattron(items[1],COLOR_PAIR(6));
						waddstr(items[1],"H");
						wattroff(items[1],COLOR_PAIR(6));
						case 1: wmove(items[2], 0,0);
						wattron(items[2],COLOR_PAIR(6));
						waddstr(items[2],"A");
						wattroff(items[2],COLOR_PAIR(6));
						break;
					}
					wnoutrefresh(items[selected+1]);
                    if (key==KEY_DOWN) 
                    {
						selected=(selected+1) % count;
                    } 
                    else 
                    {
                        selected=(selected+count-1) % count;
                    }
                    wbkgd(items[selected+1],COLOR_PAIR(1));
					switch (selected)
					{
						case 0: wmove(items[1], 0,0);
						wattron(items[1],COLOR_PAIR(7));
						waddstr(items[1],"H");
						wattroff(items[1],COLOR_PAIR(7));
						case 1: wmove(items[2], 0,0);
						wattron(items[2],COLOR_PAIR(7));
						waddstr(items[2],"A");
						wattroff(items[2],COLOR_PAIR(7));
						break;
					}
					wnoutrefresh(items[selected+1]);
                    doupdate();
				} 
				else if (key==ESCAPE) 
				{
					return -1;
                } 
                else if (key==ENTER) 
                {
                    return selected;
                }
                if (key==KEY_RESIZE)
                {
					resize_refresh();
					return -2;
				}
        }
}
WINDOW **GUI::draw_menu(){
    WINDOW **items;
    items=(WINDOW **)malloc(6*sizeof(WINDOW *));

    items[0]=newwin(7,19,1,0);
    wbkgd(items[0],COLOR_PAIR(2));
    box(items[0],ACS_VLINE,ACS_HLINE);
    items[1]=subwin(items[0],1,17,2,1);
    items[2]=subwin(items[0],1,17,3,1);
    items[3]=subwin(items[0],1,17,4,1);
	items[4]=subwin(items[0],1,17,5,1);
	items[5]=subwin(items[0],1,17,6,1);
	waddstr(items[1]," ew game");
	wattron(items[2],COLOR_PAIR(6));
	waddstr(items[2],"R");
	wattroff(items[2],COLOR_PAIR(6));
	wprintw(items[2],"eset");
	wattron(items[3],COLOR_PAIR(6));
	waddstr(items[3],"M");
	wattroff(items[3],COLOR_PAIR(6));
	wprintw(items[3],"ap editor");
	waddstr(items[4],"S");
	wattron(items[4],COLOR_PAIR(6));
	waddstr(items[4],"o");
	wattroff(items[4],COLOR_PAIR(6));
	wprintw(items[4],"lve");
	wattron(items[5],COLOR_PAIR(6));
	waddstr(items[5],"E");
	wattroff(items[5],COLOR_PAIR(6));
	wprintw(items[5],"xit");
    wbkgd(items[1],COLOR_PAIR(1));
	wmove(items[1], 0,0);
	wattron(items[1],COLOR_PAIR(7));
	waddstr(items[1],"N");
	wattroff(items[1],COLOR_PAIR(7));
    wrefresh(items[0]);
    return items;
}

WINDOW **GUI::draw_menu_help(){
    WINDOW **items;
    items=(WINDOW **)malloc(3*sizeof(WINDOW *));
    items[0]=newwin(4,19,1,15);
    wbkgd(items[0],COLOR_PAIR(2));
    box(items[0],ACS_VLINE,ACS_HLINE);
    items[1]=subwin(items[0],1,17,2,16);
    items[2]=subwin(items[0],1,17,3,16);
    waddstr(items[1]," elp_contents");
	wattron(items[2],COLOR_PAIR(6));
	waddstr(items[2],"A");
	wattroff(items[2],COLOR_PAIR(6));
	wprintw(items[2],"bout");
    wbkgd(items[1],COLOR_PAIR(1));
	wmove(items[1], 0,0);
	wattron(items[1],COLOR_PAIR(7));
	waddstr(items[1],"H");
	wattroff(items[1],COLOR_PAIR(7));
    wrefresh(items[0]);
    return items;
}

WINDOW **GUI::maps_win(){
    WINDOW **items;
    items=(WINDOW **)malloc((Files.size()+1)*sizeof(WINDOW *));
    items[0]=subwin(game_win,termxy.second-5,termxy.first-6,4,3);//игровое поле
	wbkgd(items[0],COLOR_PAIR(2));
	box(items[0],ACS_VLINE,ACS_HLINE);
	for(unsigned int i=0; i<Files.size(); i++)
	{
		items[i+1]=subwin(items[0],1,17,i+8,5);
		waddstr(items[i+1],Files[i].c_str());
	}
	wbkgd(items[1],COLOR_PAIR(1));
	
	move(5,termxy.first/2-10);
	printw("Please select a file:");
	
    wrefresh(items[0]);
    return items;
}

WINDOW **GUI::draw_enter_commands(){
    WINDOW **items;
    items=(WINDOW **)malloc(3*sizeof(WINDOW *));
    items[0]=newwin(6,termxy.first-6,termxy.second/2-3,3);
    wbkgd(items[0],COLOR_PAIR(2));
    box(items[0],ACS_VLINE,ACS_HLINE);
    items[1]=subwin(items[0],1,30,termxy.second/2-2,4);
    items[2]=subwin(items[0],3,termxy.first-8,termxy.second/2-1,4);
    waddstr(items[1],"Please, enter commands:");
	wbkgd(items[2],COLOR_PAIR(8));
	commands_line=items[2];
    wrefresh(items[0]);
    return items;
}

void GUI::draw_game_name(){
	werase(stdscr);
	gamename=subwin(stdscr,3,20,1,termxy.first/2-10);
	wbkgd(gamename,COLOR_PAIR(1));
	box(gamename,ACS_VLINE|A_BOLD,ACS_HLINE|A_BOLD);
	attron(A_BLINK|A_BOLD|A_UNDERLINE);
	move(2,termxy.first/2-4);
	printw("SUPAPLEX");
	attroff(A_BLINK|A_BOLD|A_UNDERLINE);
}

void GUI::GetListOfFiles()
{
     const char *dirname="./Maps";
     DIR *dir;
     struct dirent *entry;
     struct stat statinfo;
     if ((dir = opendir(dirname)) == NULL) 
     {
		return;
     }
     chdir(dirname);
     while ((entry = readdir(dir)) != NULL) 
     {
		stat(entry->d_name, &statinfo);
		if (S_ISREG(statinfo.st_mode)) 
		{
			Files.push_back(entry->d_name);
		}
     }
     closedir(dir);
}

