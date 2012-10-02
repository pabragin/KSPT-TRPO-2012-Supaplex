#include "GUI-ascii.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>


#define ENTER 10
#define ESCAPE 27
GUI::GUI(){
	int key, selected_from_m, selected_from_h;

	init_curses();
	ESCDELAY=0;
	x = getmaxx(stdscr);//terminal size
	y = getmaxy(stdscr);
	game_win=subwin(stdscr,y-5,x-4,4,2);//size of game window
	wbkgd(game_win,COLOR_PAIR(2));

	draw_game_name();
	draw_menubar();
	current_window=0;//current window is empty
	do
	{
        WINDOW **menu_items, **help_items;
	key=getch();
	if (key==KEY_F(1)) {	
        	menu_items=draw_menu();
		selected_from_m=scroll_menu(menu_items);
		delete_menu(menu_items, 6);
		if (selected_from_m==0){
			current_window=1;
                	resize_refresh();
		}
	}
	else if (key==KEY_F(2)) {
                help_items=draw_menu_help();
            	selected_from_h=scroll_help(help_items);
		delete_menu(help_items, 3);
		if (selected_from_h==0){
			current_window=2;
			resize_refresh();	
		}
		else if (selected_from_h==1){
			current_window=3;
			resize_refresh();
		}
	}
	hotkeys(key);
	if (key==11){
	  if(current_window==1){
	  draw_enter_commands();
	  input_Line();
	  }
	}
	//printw("%i",key);//print number of char
	if (key==KEY_RESIZE){
	  resize_refresh();
	}
	}while (selected_from_m!=4&&key!=5);
    	endwin();
}

bool GUI::hotkeys(int key){
	switch (key){
	  case 14:
	    current_window=1;
	    resize_refresh();
	    return true;
	  case 8:
	    current_window=2;
	    resize_refresh();
	    return true;
	  case 1:
	    current_window=3;
	    resize_refresh();
	    return true;
	  default:
	    return false;
	}   
}
string GUI::input_Line(){
	int key;
	int xx;
	int yy;
	string s;
	while(key!=ESCAPE && key!=KEY_RESIZE){
	key=getch();
	switch (key){
	  case 'u': waddstr(commands_line,"U");
		    wrefresh(commands_line);
		    s+=key;
		    break;
	  case 'd': waddstr(commands_line,"D");
		    wrefresh(commands_line);
		    s+=key;
		    break;
	  case 'l': waddstr(commands_line,"L");
		    wrefresh(commands_line);
		    s+=key;
		    break;
	  case 'r': waddstr(commands_line,"R");
		    wrefresh(commands_line);
		    s+=key;
		    break;
	  case 263 : getyx(commands_line, yy, xx);
		     if (yy!=0 && xx==0){
		       yy=yy-1;
		       xx=getmaxx(commands_line);
		     }
		     mvwaddstr(commands_line, yy, xx-1, " ");
		     s=s.substr(0,s.size()-1);
		     wmove(commands_line, yy, xx-1);
		     wrefresh(commands_line);
		     break;
	  case ENTER :resize_refresh(); 
		      return s;
	}
	}
	resize_refresh(); 
	return NULL;
}
void GUI::resize_refresh(){
	x = getmaxx(stdscr);//terminal size
	y = getmaxy(stdscr);
	if(x>5 && y>10){
	draw_game_name();
	draw_menubar();
	if (current_window==1){
		WINDOW **gw = draw_game_win();
		string s= "1234567890123456789012345678901234567890123456789012345678901234567";
		draw_points(100, 10, 1, s.c_str(), gw);
		char **map;
		map = (char**)malloc(9);
		map[0] = "#R#";
		map[1] = "###";
		map[2] = "#L#";
		draw_map(map, 3, 3, gw[0]);
		}
	else if (current_window==2){
		game_win=subwin(stdscr,y-5,x-4,4,2);//size of game window
		wbkgd(game_win,COLOR_PAIR(2));
		help_game_win();
	}
	else if (current_window==3){
		game_win=subwin(stdscr,y-5,x-4,4,2);//size of game window
		wbkgd(game_win,COLOR_PAIR(2));
		about_game_win();
	}
	else if (current_window==4){
		WINDOW **gw = draw_game_win();
		string s= "1234567890123456789012345678901234567890123456789012345678901234567";
		draw_points(100, 10, 1, s.c_str(), gw);
		char **map;
		map = (char**)malloc(9);
		map[0] = "#R#";
		map[1] = "###";
		map[2] = "#L#";
		draw_map(map, 3, 3, gw[0]);
		}
	}
	touchwin(stdscr);
	refresh();

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
        curs_set(0);
        noecho();
        keypad(stdscr,TRUE);
}

void GUI::draw_menubar(){
	bkgd(COLOR_PAIR(1));
	menubar=subwin(stdscr,1,x,0,0);
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
	game_win=subwin(stdscr,y-5,x-4,4,2);//size of game window
	wbkgd(game_win,COLOR_PAIR(2));
	WINDOW **frames;
	frames=(WINDOW **)malloc(3*sizeof(WINDOW *));
	frames[0]=subwin(game_win,y-8,x-20,4,2);//игровое поле
	wbkgd(frames[0],COLOR_PAIR(2));
	box(frames[0],ACS_VLINE,ACS_HLINE);
	frames[1]=subwin(game_win,y-8,16,4,x-18);//очки
	wbkgd(frames[1],COLOR_PAIR(2));
	box(frames[1],ACS_VLINE,ACS_HLINE);
	frames[2]=subwin(game_win,3,x-4,y-4,2);//ходы
	wbkgd(frames[2],COLOR_PAIR(2));
	box(frames[2],ACS_VLINE,ACS_HLINE);
	wmove(frames[1],1,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Score:");
    	wmove(frames[1],3,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Moves:");
	wmove(frames[1],5,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Lambdas:");
	wmove(frames[2],1,1);
	wattron(frames[2],COLOR_PAIR(3));
    	waddstr(frames[2],"Trace:");
	touchwin(stdscr);
	refresh();
	return frames;
}

void GUI::draw_map(char **map, int column,int row, WINDOW *game_win){
	int i = 0;
	for(i=0; i<row; i++)
	{	
		mvwprintw(game_win, i+1, 1, map[i]);
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
	if (s.size()>(x-13))
		mvwprintw(frames[2], 1,8, s.substr(s.size()-(x-13), s.size()).c_str());
	else
		mvwprintw(frames[2], 1,8, s.c_str());
	touchwin(stdscr);
        refresh();
}

void GUI::about_game_win(){
	//werase(game_win);
	WINDOW *f=subwin(game_win,y-5,x-6,4,3);//игровое поле
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
	wmove(f,5,1);
	waddstr(f,"Yarov Maxim");
}

void GUI::help_game_win(){
	//werase(field);
	WINDOW *f=subwin(game_win,y-5,x-6,4,3);//игровое поле
	wbkgd(f,COLOR_PAIR(2));
	box(f,ACS_VLINE,ACS_HLINE);
	wmove(f,1,1);
	wattron(f,COLOR_PAIR(3));
    	waddstr(f,"This game is not ready yet");
}

void GUI::delete_menu(WINDOW **items,int count){
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
			  case 3: wmove(items[4], 0,0);
				  wattron(items[4],COLOR_PAIR(6));
				  waddstr(items[4],"S");
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
			  case 3: wmove(items[4], 0,0);
				  wattron(items[4],COLOR_PAIR(7));
				  waddstr(items[4],"S");
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
		if (hotkeys(key)==true){
			return -3;
		}
        }
}
int GUI::scroll_help(WINDOW **items){
        int key;
        int selected=0;
	int count =2;
        while (1) {
                key=getch();
                if (key==KEY_DOWN || key==KEY_UP) {
                        wbkgd(items[selected+1],COLOR_PAIR(2));
			switch (selected){
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
                        if (key==KEY_DOWN) {
                                selected=(selected+1) % count;
                        } else {
                                selected=(selected+count-1) % count;
                        }
                        wbkgd(items[selected+1],COLOR_PAIR(1));
			switch (selected){
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
                } else if (key==ESCAPE) {
                        return -1;
                } else if (key==ENTER) {
                        return selected;
                }
                if (key==KEY_RESIZE){
			resize_refresh();
			return -2;
		}
		if (hotkeys(key)==true){
			return -3;
		}
        }
}
WINDOW **GUI::draw_menu(){
        int i;
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
	wattron(items[4],COLOR_PAIR(6));
	waddstr(items[4],"S");
	wattroff(items[4],COLOR_PAIR(6));
	wprintw(items[4],"olve");
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
        int i;
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

WINDOW **GUI::draw_enter_commands(){
        int i;
        WINDOW **items;
        items=(WINDOW **)malloc(3*sizeof(WINDOW *));
        items[0]=newwin(6,x-6,y/2-3,3);
        wbkgd(items[0],COLOR_PAIR(2));
        box(items[0],ACS_VLINE,ACS_HLINE);
        items[1]=subwin(items[0],1,30,y/2-2,4);
        items[2]=subwin(items[0],3,x-8,y/2-1,4);
        waddstr(items[1],"Please, enter commands:");
	wbkgd(items[2],COLOR_PAIR(8));
	commands_line=items[2];
        wrefresh(items[0]);
        return items;
}

int GUI::enter_commands_line(WINDOW **items){
        int key;
        int selected=0;
	int count =2;
}
void GUI::draw_game_name(){
	werase(stdscr);
	gamename=subwin(stdscr,3,20,1,x/2-10);
	wbkgd(gamename,COLOR_PAIR(1));
	box(gamename,ACS_VLINE|A_BOLD,ACS_HLINE|A_BOLD);
	attron(A_BLINK|A_BOLD|A_UNDERLINE);
	move(2,x/2-4);
	printw("SUPAPLEX");
	attroff(A_BLINK|A_BOLD|A_UNDERLINE);
}

